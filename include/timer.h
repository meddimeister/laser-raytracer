#pragma once

#include <chrono>
#include <tuple>

using namespace std;

class Timer {
public:
  tuple<double, double> get() {
		static auto start = chrono::high_resolution_clock::now();
		static auto last = start;
    auto now = chrono::high_resolution_clock::now();
    chrono::duration<double> absolute = now - start;
    chrono::duration<double> delta = now - last;
    last = now;
    return {absolute.count(), delta.count()};
  }
};
