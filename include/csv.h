#pragma once

#include "metaprog.h"
#include <cstddef>
#include <fstream>
#include <functional>
#include <ios>
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
  size_t precision;

public:
  CSVWriter(const string &_outputDirectory, size_t _precision = 7)
      : outputDirectory(_outputDirectory), precision(_precision) {}

  template <typename... Args> void add(const string &name, Args &&...args) {
    stringstream ss;
    ss.precision(precision);
    ss << fixed;
    constexpr_for([&](auto const &arg) { ss << arg << " "; }, args...);
    string line = ss.str();
    line = line.substr(0, line.size()-1);
    tables[name].push_back(line);
  }

  void write() const;
};

class CSVReader {
private:
  size_t precision;
public:
  CSVReader(size_t _precision = 7)
      : precision(_precision) {}
  template <typename... Ts> vector<tuple<Ts...>> read(const string &filename) {
    ifstream fs(filename);
    vector<tuple<Ts...>> lines;
    string line;
    while (std::getline(fs, line)) {
      vector<string> lineValues;
      stringstream ss(line);
      ss.precision(precision);
      ss << fixed;
      tuple<Ts...> lineTuple;
      constexpr size_t length = tuple_size_v<tuple<Ts...>>;
      constexpr_for<0, length, 1>(
          [&](auto i) { ss >> get<i.value>(lineTuple); });
      lines.push_back(lineTuple);
    }
    return lines;
  }
};