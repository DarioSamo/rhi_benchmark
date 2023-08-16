//
// RHI
//

#include "timer.h"

// Timer

LARGE_INTEGER Timer::Frequency;
bool Timer::FrequencyInitialized = false;

void Timer::initialize() {
	QueryPerformanceFrequency(&Timer::Frequency);
	Timer::FrequencyInitialized = true;
}

Timestamp Timer::current() {
	LARGE_INTEGER ts;
	QueryPerformanceCounter(&ts);
	return ts;
}

int64_t Timer::deltaMicroseconds(const Timestamp t1, const Timestamp t2) {
	int64_t deltaTime;
	deltaTime = t2.QuadPart - t1.QuadPart;
	deltaTime *= 1000000;
	deltaTime /= Timer::Frequency.QuadPart;
	return deltaTime;
}

// ElapsedTimer

ElapsedTimer::ElapsedTimer() {
	assert(Timer::FrequencyInitialized);
	reset();
}

void ElapsedTimer::reset() {
	startTime = Timer::current();
}

int64_t ElapsedTimer::elapsedMicroseconds() const {
	return Timer::deltaMicroseconds(startTime, Timer::current());
}

double ElapsedTimer::elapsedMilliseconds() const {
	return static_cast<double>(elapsedMicroseconds()) / 1000.0;
}

double ElapsedTimer::elapsedSeconds() const {
	return static_cast<double>(elapsedMicroseconds()) / 1000000.0;
}