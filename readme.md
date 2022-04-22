# BM #
Minimalist benchmarking library.

## Abstractions ##

#### `bm::record<type>` #####
Simple struct containing a vector. 
Provides functionality to compute the mean, variance and standard deviation. Exports to csv.

```cpp
template<typename type = double>
struct record
{
  type mean              () {...}
  type variance          () {...}
  type standard_deviation() {...}

  void to_csv 			 (const std::string& filepath) {...}
  
  std::vector<type> values;
}
```

#### `bm::session<type>` ####
Simple struct containing a flat map of records, indexed by a name. 
Exports to csv. 
Only used for macro-benchmarking.

```cpp
template<typename type = double>
struct session
{
  void to_csv(const std::string& filepath, const bool include_name = true) {...}
  
  std::vector<std::pair<std::string, record<type>>> records;
}
```

#### `bm::session_recorder<type, period>` ####
Helper class providing a single public method accepting a name and a function. 
The function is run once, and its duration is appended to an internally managed session indexed by name. 
Only used for macro-benchmarking.

```cpp
template <typename type = double, typename period = std::milli>
class session_recorder
{
public:
  void record(const std::string& name, const std::function<void()>& function) const {...}
}

```

#### `bm::run<type, period>` ####
The entry function which runs a benchmark and creates records / sessions. Provides two overrides for micro- and macro-benchmarking.

```cpp
template<typename type = double, typename period = std::milli>
record<type> run(const std::size_t iterations, const std::function<void()>& function) {...}

template<typename type = double, typename period = std::milli>
session<type> run(const std::size_t iterations, const std::function<void(session_recorder<type, period>&)>& function) {...}
```

## Example Usage ##

```cpp
#include <algorithm>
#include <cstddef>
#include <vector>

#include <bm/bm.hpp>

int main(int argc, char** argv)
{
  std::vector<std::size_t> buffer(100000);

  const auto record = bm::run<float, std::milli>([&]
  {
    std::iota(buffer.begin(), buffer.end(), 0);
  }, 100 /* iterations */);
  auto mean               = record.mean              ();
  auto variance           = record.variance          ();
  auto standard_deviation = record.standard_deviation();
  record.to_csv("output_single.csv");

  const auto session = bm::run<float, std::milli>([&buffer] (auto& recorder)
  {
    recorder.record("iota"    , [&buffer]
    {
      std::iota(buffer.begin(), buffer.end(), 0);
    });
    recorder.record("generate", [&buffer]
    {
      std::generate(buffer.begin(), buffer.end(), std::rand);
    });
  }, 100 /* iterations */);
  for (const auto& record : session.records)
  {
    auto name               = record.name;
    auto mean               = record.mean              ();
    auto variance           = record.variance          ();
    auto standard_deviation = record.standard_deviation();
  }
  session.to_csv("output_multi.csv");
}
```

Sample output_single.csv:
```
name,run_0,run_1,run_2,run_3,run_4,run_5,run_6,run_7,run_8,run_9,mean,variance,standard deviation
benchmark,0.289900005,0.2852,0.295899987,0.287999988,0.288399994,0.287,0.296400011,0.280900002,0.264800012,0.281699985,0.285819978,7.26395519e-05,0.00852288399
```

Sample output_multi.csv:
```
name,run_0,run_1,run_2,run_3,run_4,run_5,run_6,run_7,run_8,run_9,mean,variance,standard deviation
iota,0.292800009,0.318199992,0.326999992,0.293300003,0.298500001,0.281300008,0.242699996,0.266900003,0.292899996,0.273400009,0.288700044,0.000536887906,0.0231708419
generate,14.5121002,15.6627998,13.3808002,12.7665005,12.0438995,11.6500998,11.5285997,12.4758997,12.7679996,11.9224005,12.8711109,1.58014572,1.25703847
```