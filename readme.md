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
  auto session = bm::run<float, std::milli>(100, [&buffer] (bm::session_recorder<float, std::milli>& recorder)
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
```

Sample output iota_100000_values_100_iterations.csv:
```
0.0906620026, 0.113328002, 0.0902350023, 0.087669, 0.0795440003, 0.0615819991, 0.0620100014, 0.0739839971, 0.0615819991, 0.0620100014, 0.0615819991, 0.0615819991, 0.0620100014, 0.0838200003, 0.064576, 0.0615819991, 0.062438, 0.0615819991, 0.0620100014, 0.0615830012, 0.0620089993, 0.0620100014, 0.0765499994, 0.0910900012, 0.104776002, 0.106913, 0.108195998, 0.106486, 0.108623996, 0.0957949981, 0.0987880006, 0.0962219983, 0.0975050032, 0.065857999, 0.0620100014, 0.0615819991, 0.0765499994, 0.0684249997, 0.0791160017, 0.0722730011, 0.0658589974, 0.0658589974, 0.0688519999, 0.0620100014, 0.0615819991, 0.076977998, 0.109907001, 0.101354003, 0.0868140012, 0.0803989992, 0.0803980008, 0.106486, 0.0936560035, 0.0910909995, 0.0842479989, 0.0786880031, 0.0855309963, 0.0902350023, 0.0927999988, 0.106913999, 0.0992150009, 0.0983610004, 0.0970769972, 0.0936560035, 0.097078003, 0.0829640031, 0.0898080021, 0.0928009972, 0.100497998, 0.0910909995, 0.0962219983, 0.0945110023, 0.0975050032, 0.0966499969, 0.0966499969, 0.0975050032, 0.0966499969, 0.0975050032, 0.0996439978, 0.106485002, 0.0932290033, 0.0936560035, 0.0932290033, 0.0936560035, 0.0928009972, 0.0936560035, 0.0923729986, 0.0936560035, 0.0932290033, 0.0851029977, 0.0966489986, 0.160371006, 0.0992159992, 0.097078003, 0.103491999, 0.105631001, 0.106913999, 0.104346998, 0.103064999, 0.100070998
```

Sample output first_iota_then_generate_100000_values_100_iterations.csv:
```
iota, 0.125303, 0.136420995, 0.295509011, 0.127012998, 0.0620100014, 0.081253998, 0.0620100014, 0.110762, 0.121881999, 0.109050997, 0.0808269978, 0.0709900036, 0.0611549988, 0.146685004, 0.102637, 0.0739839971, 0.104774997, 0.0966489986, 0.115893997, 0.110762998, 0.138558999, 0.216392994, 0.0996439978, 0.0688519999, 0.0628649965, 0.0641489998, 0.0650030002, 0.595293999, 0.098360002, 0.101781003, 0.109052002, 0.0987880006, 0.100925997, 0.104774997, 0.111189999, 0.0957939997, 0.123591997, 0.112901002, 0.104774997, 0.128297001, 0.154383004, 0.0692799985, 0.0650039986, 0.0650039986, 0.110335, 0.102209002, 0.120598003, 0.237348005, 0.119314998, 0.097078003, 0.129151002, 0.117177002, 0.103064999, 0.121026002, 0.133855999, 0.116749004, 0.105630003, 0.115038998, 0.101354003, 0.0992150009, 0.123163998, 0.117605001, 0.124448001, 0.115895003, 0.137277007, 0.0936560035, 0.107340999, 0.0975050032, 0.0949390009, 0.109479003, 0.0718460009, 0.0692799985, 0.0654309988, 0.0620100014, 0.0607270002, 0.065857999, 0.065857999, 0.0607270002, 0.0928009972, 0.0697079971, 0.0611549988, 0.0602990016, 0.0671420023, 0.0855309963, 0.113755003, 0.101354003, 0.0957939997, 0.105630003, 0.120599002, 0.117177002, 0.112044998, 0.120171003, 0.104347996, 0.120599002, 0.100925997, 0.0979330018, 0.128723994, 0.0945120007, 0.114184, 0.100925997
generate, 4.26413584, 3.75223398, 3.74924111, 2.36364198, 2.22166109, 2.20156193, 2.88922906, 3.39086699, 3.94553399, 3.82237005, 2.22251606, 2.12543988, 3.36905599, 3.67397308, 3.50120091, 2.82037711, 3.62393808, 3.26984096, 3.77618289, 4.78758383, 3.8980639, 5.51887178, 2.867419, 2.60056305, 2.37818193, 2.35295105, 2.96406889, 5.00825405, 52.4979286, 3.60597706, 3.68552089, 3.98316693, 4.30519104, 17.7989521, 3.82621908, 16.5647411, 4.31588221, 4.35650921, 4.09179115, 4.51089191, 3.20825911, 2.37732792, 2.30377102, 3.93056607, 5.18316412, 4.77475405, 4.14866877, 3.9844501, 4.08580494, 3.88523507, 13.9723053, 4.50875378, 21.1269512, 4.37789202, 8.81907558, 3.90020299, 3.88480711, 3.81638193, 5.23148918, 3.92158508, 4.55451298, 4.14268303, 4.16363811, 4.38088608, 3.6316359, 3.94382405, 6.57303905, 4.68109798, 3.77404499, 3.49393201, 3.01838112, 2.13955212, 2.25672889, 2.16563892, 2.30590892, 2.15665793, 2.19386411, 2.78103209, 3.60383892, 2.17248201, 2.16563797, 2.1575141, 2.59628701, 4.39157677, 3.61752391, 3.51403093, 3.62265491, 7.20211697, 6.57988214, 3.81809306, 4.33213282, 3.85358906, 3.75693893, 3.95109391, 3.82707405, 4.16748714, 3.96691704, 3.87240505, 3.86043096, 3.74881291
```