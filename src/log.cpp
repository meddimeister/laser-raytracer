#include "log.h"
#include "timer.h"
#include <iostream>
#include <iomanip>

Timer logtimer;
Timer debugtimer;

void LOG(const string &s)
{
	cout.setf(ios::scientific | ios::left);
	const auto [absolute, delta] = logtimer.get();
	string timerPrompt = "[" + to_string(absolute) + "s" + ", " + to_string(delta) + "s" + "]";
	cout << setw(25) << timerPrompt << " " << s << endl;
	cout.unsetf(ios::scientific | ios::left);
}

void DEBUG(const string &s)
{
	cout.setf(ios::scientific | ios::left);
	const auto [absolute, delta] = logtimer.get();
	string timerPrompt = "\033[31m[" + to_string(absolute) + "s" + ", " + to_string(delta) + "s" + "]\033[0m";
	cout << setw(25) << timerPrompt << " " << s << endl;
	cout.unsetf(ios::scientific | ios::left);
}