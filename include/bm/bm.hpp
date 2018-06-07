#ifndef BM_BENCHMARK_HPP_
#define BM_BENCHMARK_HPP_

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <fstream>
#include <functional>
#include <limits>
#include <numeric>
#include <string>
#include <vector>

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
    auto m = mean();
    std::vector<type> differences(values.size());
    std::transform(values.begin(), values.end(), differences.begin(), [m] (const type& value) { return value - m; });
    return std::inner_product(differences.begin(), differences.end(), differences.begin(), type(0)) / values.size();
  }
  type standard_deviation()
  {
    return std::sqrt(variance());
  }

  void to_csv            (const std::string& filepath)
  {
    std::ofstream file(filepath);
    file.precision(std::numeric_limits<type>::max_digits10);
    
    auto size = values.size();
    for (auto j = 0; j < size; ++j)
    {
      file << values[j];
      if (j != size - 1) 
        file << ", ";
    }
  }

  std::vector<type> values;
};

template <typename type = double>
struct session
{
  void to_csv(const std::string& filepath, const bool include_name = true)
  {
    std::ofstream file(filepath);
    file.precision(std::numeric_limits<type>::max_digits10);

    for (auto& kvp : records)
    {
      if (include_name) file << kvp.first << ", ";
      
      auto& values = kvp.second.values;
      auto  size   = values.size();

      for (auto j = 0; j < size; ++j)
      {
        file << values[j];
        if (j != size - 1) 
          file << ", ";
      }
      
      file << "\n";
    }
  }

  std::vector<std::pair<std::string, record<type>>> records;
};

template <typename type = double, typename period = std::milli>
class  session_recorder
{
public:
  explicit session_recorder  (const std::size_t index, const std::size_t iterations, session<type>& session) 
  : index_(index), iterations_(iterations), session_(session)
  {

  }
  session_recorder           (const session_recorder&  that) = delete ;
  session_recorder           (      session_recorder&& temp) = default;
  virtual ~session_recorder  ()                      = default;
  session_recorder& operator=(const session_recorder&  that) = delete ;
  session_recorder& operator=(      session_recorder&& temp) = default;
  
  void record(const std::string& name, const std::function<void()>& function)
  {
    const auto start = std::chrono::high_resolution_clock::now();
    function();
    const auto end   = std::chrono::high_resolution_clock::now();

    // There isn't a cleaner way to do this without boost::multi_index...
    // ...which is an unacceptible dependency for ~200 lines of code.
    auto record = std::find_if(session_.records.begin(), session_.records.end(), 
      [&name] (const std::pair<std::string, bm::record<type>>& record) { return record.first == name; });
    if (record == session_.records.end())
    {
      session_.records.push_back({name, {std::vector<type>(iterations_)}});
      record = std::prev(session_.records.end());
    }
    record->second.values[index_] = std::chrono::duration<type, period>(end - start).count();
  }

protected:
  const std::size_t index_     ;
  const std::size_t iterations_;
  session<type>&    session_   ;
};

template<typename type = double, typename period = std::milli>
record<type>  run(const std::size_t iterations, const std::function<void()>&                                function)
{
  record<type> record {std::vector<type>(iterations)};
  for (auto i = 0; i < iterations; ++i)
  {
    const auto start = std::chrono::high_resolution_clock::now();
    function();
    const auto end   = std::chrono::high_resolution_clock::now();
    record.values[i] = std::chrono::duration<type, period>(end - start).count();
  }
  return record;
}
template<typename type = double, typename period = std::milli>
session<type> run(const std::size_t iterations, const std::function<void(session_recorder<type, period>&)>& function)
{
  session<type> session;
  for(auto i = 0; i < iterations; ++i)
  {
    session_recorder<type, period> recorder(i, iterations, session);
    function(recorder);
  }
  return session;
}

template<typename type = double, typename period = std::milli>
type          run(                              const std::function<void()>&                                function)
{
  return run<type, period>(std::size_t(1), function)[0];
}
template<typename type = double, typename period = std::milli>
session<type> run(                              const std::function<void(session_recorder<type, period>&)>& function)
{
  return run<type, period>(std::size_t(1), function);
}
}

#endif