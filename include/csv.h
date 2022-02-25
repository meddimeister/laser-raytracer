#pragma once

#include <cstddef>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <vector>
#include "metaprog.h"

using namespace std;

class CSVWriter {
private:
  string outputDirectory;
  unordered_map<string, vector<string>> tables;

public:
  CSVWriter(const string &_outputDirectory)
      : outputDirectory(_outputDirectory) {}

  void add(const string &line, const string &name);

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