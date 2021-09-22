#pragma once

#include <fstream>
#include <iostream>
#include <unordered_map>

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