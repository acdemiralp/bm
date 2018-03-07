#include "catch.hpp"

#include <bm/benchmark.hpp>

std::vector<std::size_t> iota    (const std::size_t size)
{
  std::vector<std::size_t> vector(size);
  std::iota(vector.begin(), vector.end(), 0);
  return vector;
}
std::vector<std::size_t> generate(const std::size_t size)
{
  std::vector<std::size_t> vector(size);
  std::generate(vector.begin(), vector.end(), std::rand);
  return vector;
}

TEST_CASE("Benchmark Test", "[benchmark]")
{
  auto session = bm::run<float, std::milli>(100, [ ] (bm::recorder<float, std::milli>& recorder)
  {
    recorder.record("1-iota"    , [ ] ( )
    {
      auto values = iota    (100000);
    });
    recorder.record("2-generate", [ ] ( )
    {
      auto values = generate(100000);
    });
  });
  session.to_csv("functionx100.csv", true);
}