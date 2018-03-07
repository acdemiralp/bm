#ifndef BM_BENCHMARK_HPP_
#define BM_BENCHMARK_HPP_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <numeric>
#include <string>
#include <unordered_map>
#include <vector>

#include <boost/lexical_cast.hpp>

namespace bm
{
template <typename type = double>
struct record
{
  type mean              () 
  {
    return std::accumulate(values.begin(), values.end(), type(0)) / values.size();
  }
  type variance          ()
  {
    auto mean = mean();
    std::vector<type> differences(values.size());
    std::transform(values.begin(), values.end(), differences.begin(), [mean] (const type& value) { return value - mean; });
    return std::inner_product(differences.begin(), differences.end(), differences.begin(), type(0)) / values.size();
  }
  type standard_deviation()
  {
    return std::sqrt(variance());
  }

  std::vector<type> values;
};

template <typename type = double>
struct session
{
  void to_csv(const std::string& filepath, const bool include_name = false)
  {
    std::ofstream file(filepath);
    for (auto& kvp : records)
    {
      auto& values = kvp.second.values;
      auto  size   = values.size();

      if (include_name) file << kvp.first << ", ";
      
      for (auto j = 0; j < size; ++j)
      {
        file << boost::lexical_cast<std::string>(values[j]);
        if (j != size - 1) file << ", ";
      }
      
      file << "\n";
    }
  }

  std::unordered_map<std::string, record<type>> records;
};

template <typename type = double, typename period = std::milli>
class recorder
{
public:
  explicit recorder  (const std::size_t index, const std::size_t iterations, session<type>& session) : index_(index), iterations_(iterations), session_(session)
  {

  }
  recorder           (const recorder&  that) = delete ;
  recorder           (      recorder&& temp) = default;
  virtual ~recorder  ()                      = default;
  recorder& operator=(const recorder&  that) = delete ;
  recorder& operator=(      recorder&& temp) = default;
  
  void record(const std::string& name, const std::function<void()>& function) const
  {
    const auto start = std::chrono::high_resolution_clock::now();
    function();
    const auto end   = std::chrono::high_resolution_clock::now();

    if (index_ == 0) session_.records[name].values.resize(iterations_);
    session_.records[name].values[index_] = std::chrono::duration<type, period>(end - start).count();
  }

protected:
  const std::size_t index_     ;
  const std::size_t iterations_;
  session<type>&    session_   ;
};

template<typename type = double, typename period = std::milli>
session<type> run(const std::size_t iterations, const std::function<void(recorder<type, period>&)>& function)
{
  session<type> session;
  for(auto i = 0; i < iterations; ++i)
  {
    recorder<type, period> recorder(i, iterations, session);
    function(recorder);
  }
  return session;
}
}

#endif