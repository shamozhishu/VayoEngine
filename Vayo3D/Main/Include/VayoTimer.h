/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 定时器
\*************************************************************************/
#ifndef __VAYO_TIMER_H__
#define __VAYO_TIMER_H__

#include "VayoCommon.h"
NS_VAYO_BEGIN

class _VayoExport Timer
{
public:
	Timer();
	bool isStopped() const { return _stopped; }

	float totalTime() const;  // in seconds
	float deltaTime() const; // in seconds

	unsigned int totalMilliTime() const; // in milliseconds
	unsigned int deltaMilliTime() const; // in milliseconds

	void reset(); // Call before message loop.
	void start(); // Call when unpaused.
	void stop();  // Call when paused.
	void tick();  // Call every frame.

private:
	bool _stopped;

	double _secondsPerCount;
	double _deltaTime;

	__int64 _baseTime;
	__int64 _pausedTime;
	__int64 _stopTime;
	__int64 _prevTime;
	__int64 _currTime;
};

NS_VAYO_END

#endif // __VAYO_TIMER_H__
