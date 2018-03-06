#ifndef BM_BENCHMARK_HPP_
#define BM_BENCHMARK_HPP_

#include <algorithm>
#include <chrono>
#include <functional>
#include <numeric>
#include <type_traits>
#include <vector>

namespace bm
{
template <typename type, typename enable = void>
struct results;
template <typename type, typename std::enable_if<!std::is_arithmetic<type>::value>::type>
struct results
{
  std::vector<type> measurements;
};
template <typename type, typename std::enable_if< std::is_arithmetic<type>::value>::type>
struct results
{
  type mean              () 
  {
    return std::accumulate(measurements.begin(), measurements.end(), type(0)) / measurements.size();
  }
  type variance          ()
  {
    auto mean = mean();
    std::vector<type> differences(measurements.size());
    std::transform(measurements.begin(), measurements.end(), differences.begin(), [mean] (const type& value) { return value - mean; });
    return std::inner_product(differences.begin(), differences.end(), differences.begin(), type(0)) / measurements.size();
  }
  type standard_deviation()
  {
    return std::sqrt(variance());
  }

  std::vector<type> measurements;
};

template<typename function_type>
class benchmark
{
public:
  explicit benchmark  (const std::function<function_type>& function) : function_(function)
  {

  }
  benchmark           (const benchmark&  that) = default;
  benchmark           (      benchmark&& temp) = default;
  virtual ~benchmark  ()                       = default;
  benchmark& operator=(const benchmark&  that) = default;
  benchmark& operator=(      benchmark&& temp) = default;

  void run(const std::size_t iterations)
  {

  }

protected:
  std::function<function_type> function_;
};
}

#endif