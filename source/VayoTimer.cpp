#include "VayoTimer.h"
#include <Windows.h>

NS_VAYO_BEGIN

float Timer::deltaTime() const
{
	return (float)_deltaTime;
}

Timer::Timer()
	: _secondsPerCount(0.0)
	, _deltaTime(-1.0)
	, _baseTime(0)
	, _pausedTime(0)
	, _prevTime(0)
	, _currTime(0)
	, _stopped(false)
{
	__int64 countsPerSec;
	QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);
	_secondsPerCount = 1.0 / (double)countsPerSec;
}

// Returns the total time elapsed since Reset() was called, NOT counting any
// time when the clock is stopped.
float Timer::totalTime() const
{
	// If we are stopped, do not count the time that has passed since we stopped.
	// Moreover, if we previously already had a pause, the distance
	// mStopTime - mBaseTime includes paused time, which we do not want to count.
	// To correct this, we can subtract the paused time from mStopTime:
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime

	if (_stopped)
	{
		return (float)(((_stopTime - _pausedTime) - _baseTime)*_secondsPerCount);
	}

	// The distance mCurrTime - mBaseTime includes paused time,
	// which we do not want to count.  To correct this, we can subtract
	// the paused time from mCurrTime:
	//
	//  (mCurrTime - mPausedTime) - mBaseTime
	//
	//                     |<--paused time-->|
	// ----*---------------*-----------------*------------*------> time
	//  mBaseTime       mStopTime        startTime     mCurrTime

	else
	{
		return (float)(((_currTime - _pausedTime) - _baseTime)*_secondsPerCount);
	}
}

unsigned int Timer::totalMilliTime() const
{
	if (_stopped)
		return (unsigned int)(((_stopTime - _pausedTime) - _baseTime) * 1000 * _secondsPerCount);
	else
		return (unsigned int)(((_currTime - _pausedTime) - _baseTime) * 1000 * _secondsPerCount);
}

unsigned int Timer::deltaMilliTime() const
{
	return (unsigned int)(_deltaTime * 1000.0);
}

void Timer::reset()
{
	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

	_baseTime = currTime;
	_prevTime = currTime;
	_stopTime = 0;
	_stopped = false;
}

void Timer::start()
{
	__int64 startTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&startTime);


	// Accumulate the time elapsed between stop and start pairs.
	//
	//                     |<-------d------->|
	// ----*---------------*-----------------*------------> time
	//  mBaseTime       mStopTime        startTime

	if (_stopped)
	{
		_pausedTime += (startTime - _stopTime);

		_prevTime = startTime;
		_stopTime = 0;
		_stopped = false;
	}
}

void Timer::stop()
{
	if (!_stopped)
	{
		__int64 currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		_stopTime = currTime;
		_stopped = true;
	}
}

void Timer::tick()
{
	if (_stopped)
	{
		_deltaTime = 0.0;
		return;
	}

	__int64 currTime;
	QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
	_currTime = currTime;

	// Time difference between this frame and the previous.
	_deltaTime = (_currTime - _prevTime)*_secondsPerCount;

	// Prepare for next frame.
	_prevTime = _currTime;

	// Force nonnegative.  The DXSDK's CDXUTTimer mentions that if the
	// processor goes into a power save mode or we get shuffled to another
	// processor, then mDeltaTime can be negative.
	if (_deltaTime < 0.0)
	{
		_deltaTime = 0.0;
	}
}

NS_VAYO_END
