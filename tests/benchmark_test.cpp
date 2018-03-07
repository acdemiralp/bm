#include "catch.hpp"

#include <bm/benchmark.hpp>

void test_function()
{
}

TEST_CASE("Benchmark Test", "[benchmark]")
{
  auto session = bm::run<float, std::milli>(std::size_t(100), [ ] (bm::recorder<float, std::milli>& recorder)
  {
    recorder.record<void()>("Test Function", [ ] ()
    {

    });
  });
  session.to_csv("test_functionx100.csv");
}