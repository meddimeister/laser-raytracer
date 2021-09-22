#include "log.h"
#include "timer.h"
#include <iostream>
#include <iomanip>

Timer logtimer;

void setCoutParams()
{
	cout.setf(ios::scientific | ios::left);
}

void resetCoutParams()
{
	cout.unsetf(ios::scientific | ios::left);
}

void LOG(const string &s)
{
	setCoutParams();
	const auto [absolute, delta] = logtimer.get();
	string timerPrompt = "[" + to_string(absolute) + "s" + ", " + to_string(delta) + "s" + "]";
	cout << setw(25) << timerPrompt << " " << s << endl;
	resetCoutParams();
}
