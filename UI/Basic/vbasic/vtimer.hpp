/*
 * VTimer.hpp
 *	@description : Timer
 *	@birth		 : 2022/6.11
*/

#pragma once

#include "vbase.hpp"

#include <time.h>

VLIB_BEGIN_NAMESPACE

/*
 * VTimer class:
 *	@description  : The Basic Timer
*/
class VTimer {
private:
	time_t        ClockTimer;
	unsigned long Duraction;

public:
	/*
	 * Build up Functional
	*/

	VTimer() {
		Duraction  = 0;
		ClockTimer = 0;
	}

	/*
	 * Start Functional:
	 *	@description  : Start Clock
	*/
	void Start(unsigned long TimerDuraction) {
		ClockTimer = clock();

		Duraction  = TimerDuraction;
	}
	/*
	 * End Functional:
	 *	@description  : Timer End
	*/
	bool End() {
		return (clock() - ClockTimer >= Duraction);
	}
};

VLIB_END_NAMESPACE