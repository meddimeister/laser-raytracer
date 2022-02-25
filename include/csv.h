#pragma once

#include "metaprog.h"
#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>

using namespace std;

class CSVWriter {
private:
  string outputDirectory;
  unordered_map<string, vector<string>> tables;

public:
  CSVWriter(const string &_outputDirectory)
      : outputDirectory(_outputDirectory) {}

  template <typename... Args> void add(const string &name, Args &&...args) {
    stringstream ss;
    constexpr_for([&](auto const &arg) { ss << arg << " "; }, args...);
    tables[name].push_back(ss.str());
  }

  void write() const;
};

class CSVReader {
public:
  template <typename... Ts> vector<tuple<Ts...>> read(const string &filename) {
    ifstream fs(filename);
    vector<tuple<Ts...>> lines;
    string line;
    while (std::getline(fs, line)) {
      vector<string> lineValues;
      stringstream ss(line);
      tuple<Ts...> lineTuple;
      constexpr size_t length = tuple_size_v<tuple<Ts...>>;
      constexpr_for<0, length, 1>(
          [&](auto i) { ss >> get<i.value>(lineTuple); });
      lines.push_back(lineTuple);
    }
    return lines;
  }
};