//
// RT64
//

#pragma once

#include <cassert>
#include <stdint.h>

#include <Windows.h>

typedef LARGE_INTEGER Timestamp;

struct Timer {
	static LARGE_INTEGER Frequency;
	static bool FrequencyInitialized;
	static void initialize();
	static Timestamp current();
	static int64_t deltaMicroseconds(const Timestamp t1, const Timestamp t2);
};

struct ElapsedTimer {
	Timestamp startTime;

	ElapsedTimer();
	void reset();
	int64_t elapsedMicroseconds() const;
	double elapsedMilliseconds() const;
	double elapsedSeconds() const;
};