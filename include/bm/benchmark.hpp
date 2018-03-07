#ifndef BM_BENCHMARK_HPP_
#define BM_BENCHMARK_HPP_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <map>
#include <numeric>
#include <string>
#include <vector>

#include <boost/lexical_cast.hpp>

namespace bm
{
template <typename type>
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

template <typename type>
struct session
{
  void to_csv(const std::string& filepath, const bool include_name = false)
  {
    std::ofstream file(filepath);
    for (auto& kvp : records)
    {
      auto& values = kvp.second.values;
      auto& size   = values.size();

      if (include_name) file << kvp.first << ", ";
      
      for (auto j = 0; j < size; ++j)
      {
        file << boost::lexical_cast<std::string>(values[j]);
        if (j != size - 1) file << ", ";
      }
      
      file << "\n";
    }
  }

  std::map<std::string, record<type>> records;
};

template <typename type, typename period = std::milli>
class recorder
{
public:
  recorder           (session<type>& session, const std::size_t iteration) : session_(session), iteration_(iteration)
  {
    
  }
  recorder           (const recorder&  that) = delete ;
  recorder           (      recorder&& temp) = default;
  virtual ~recorder  ()                      = default;
  recorder& operator=(const recorder&  that) = delete ;
  recorder& operator=(      recorder&& temp) = default;
  
  template<typename function_type>
  void record(const std::string& name, const std::function<function_type>& function)
  {
    //session_.records[name].second
  }

protected:
  session<type>&    session_  ;
  const std::size_t iteration_;
};

template<typename type, typename period>
session<type> run(const std::size_t iterations, const std::function<void(recorder<type, period>&)>& function)
{
  session<type> session;
  for(auto i = 0; i < iterations; ++i)
  {
     
  }
  return session;
}
}

#endif