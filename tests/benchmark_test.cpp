#include "catch.hpp"

#include <algorithm>
#include <cstddef>
#include <vector>

#include <bm/bm.hpp>

TEST_CASE("Benchmark Test", "[benchmark]")
{
  std::vector<std::size_t> buffer(100000);

  // Micro-benchmarking.
  auto record = bm::run<float, std::milli>(100, [&] ()
  {
    std::iota(buffer.begin(), buffer.end(), 0);
  });
  auto mean               = record.mean              ();
  auto variance           = record.variance          ();
  auto standard_deviation = record.standard_deviation();
  record.to_csv("iota_100000_values_100_iterations.csv");

  // Macro-benchmarking.
  auto session = bm::run<float, std::milli>(100, [&buffer] (bm::recorder<float, std::milli>& recorder)
  {
    recorder.record("iota", [&buffer] ()
    {
      std::iota(buffer.begin(), buffer.end(), 0);
    });
    recorder.record("generate", [&buffer] ()
    {
      std::generate(buffer.begin(), buffer.end(), std::rand);
    });
  });
  for(auto record : session.records)
  {
    auto name               = record.first;
    auto mean               = record.second.mean              ();
    auto variance           = record.second.variance          ();
    auto standard_deviation = record.second.standard_deviation();
  }
  session.to_csv("first_iota_then_generate_100000_values_100_iterations.csv", true);
}