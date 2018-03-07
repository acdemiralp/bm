#include "catch.hpp"

#include <bm/benchmark.hpp>

std::vector<std::size_t> function(const std::size_t size)
{
  std::vector<std::size_t> vector(size);
  std::iota(vector.begin(), vector.end(), 0);
  return vector;
}

TEST_CASE("Benchmark Test", "[benchmark]")
{
  auto session = bm::run<float, std::milli>(100, [ ] (bm::recorder<float, std::milli>& recorder)
  {
    recorder.record("Test Function", [ ] ( )
    {
      auto values = function(2048);
    });
  });
  session.to_csv("test_functionx100.csv");
}