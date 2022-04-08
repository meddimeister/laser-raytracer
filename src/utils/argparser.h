#pragma once
#include "utils/metaprog.h"
#include <algorithm>
#include <exception>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

class ArgParser {
  const string _argsString;
  vector<string> _tokens;
  vector<string> _flagTokens;
  vector<string> _argsTokens;

  unordered_map<string, bool> _flags;
  unordered_map<string, string> _args;

public:
  ArgParser(const string &argsString, int argc, char **argv)
      : _argsString(argsString) {
    parseArgString(_argsString);
    readArgs(argc, argv);
    checkArgs();
  }

  void parseArgString(const string &argsString) {
    stringstream ss(argsString);
    string token;
    while (getline(ss, token, ' ')) {
      if (regex_match(token, regex("-\\w.*")))
        _flagTokens.push_back(token);
      else if (regex_match(token, regex("--\\w.*")))
        _argsTokens.push_back(token);
      else
        throw invalid_argument("Invalid argsString at: " + token);
    }
  }

  void readArgs(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
      _tokens.push_back(argv[i]);
    }
  }

  void checkArgs() {
    for (auto it = _tokens.begin(); it != _tokens.end();) {
      string token = *it;
      if (regex_match(token, regex("-\\w.*"))) {
        auto findit = find(_flagTokens.begin(), _flagTokens.end(), token);
        if (findit != _flagTokens.end()) {
          _flags[token] = true;
        } else {
          throw invalid_argument("Unknown argument: " + token);
        }
        it++;
      } else if (regex_match(token, regex("--\\w.*"))) {
        auto findit = find(_argsTokens.begin(), _argsTokens.end(), token);
        if (findit != _argsTokens.end()) {
          if ((it + 1) != _tokens.end()) {
            _args[token] = *(it + 1);
          } else {
            throw invalid_argument("Missing value for argument: " + token);
          }
        } else {
          throw invalid_argument("Unknown argument: " + token);
        }
        it += 2;
      } else {
        throw invalid_argument("Invalid argument position at: " + token);
      }
    }
  }

  template <typename T> bool getArg(const string &argName, T &out) {
    if (find(_argsTokens.begin(), _argsTokens.end(), argName) !=
        _argsTokens.end()) {
      auto it = _args.find(argName);
      if (it != _args.end()) {
        stringstream ss(it->second);
        ss >> out;
        return true;
      } else {
        return false;
      }
    } else {
      throw invalid_argument("Argument " + argName + " was not declared in [" +
                             _argsString + "]");
    }
  }

  bool getFlag(const string &flagName, bool &out) {
    if (find(_flagTokens.begin(), _flagTokens.end(), flagName) !=
        _flagTokens.end()) {
      auto it = _flags.find(flagName);
      if (it != _flags.end()) {
        out = it->second;
        return true;
      } else {
        out = false;
        return false;
      }
    } else {
      throw invalid_argument("Argument " + flagName + " was not declared in [" +
                             _argsString + "]");
    }
  }
};