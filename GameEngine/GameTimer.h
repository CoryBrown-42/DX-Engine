//***************************************************************************************
// GameTimer.h by Frank Luna (C) 2011 All Rights Reserved.
//***************************************************************************************
#pragma once

#include <Windows.h>

/*
GameTimer is for keeping track of total in game and paused time.
*/
class GameTimer
{
public:
	GameTimer(); 

	void Reset(); // Call before message loop.
	void Start(); // Call to unpause.
	void Stop();  // Call to pause.
	void Tick();  // Call every frame.

	//Returns the total time in seconds
	inline float TotalTime() const
	{
		return (float)(((mCurrTime)-mBaseTime)*mSecondsPerCount);
	}

	// Returns the total time elapsed in seconds since Reset() was called, NOT counting any
	// time when the clock is stopped.
	inline float TotalGameTime() const
	{
		// If we are stopped, do not count the time that has passed since we stopped.
		// Moreover, if we previously already had a pause, the distance 
		// mStopTime - mBaseTime includes paused time, which we do not want to count.
		// To correct this, we can subtract the paused time from mStopTime:  
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mStopTime    mCurrTime
		
		// The distance mCurrTime - mBaseTime includes paused time,
		// which we do not want to count.  To correct this, we can subtract 
		// the paused time from mCurrTime:  
		//
		//  (mCurrTime - mPausedTime) - mBaseTime 
		//
		//                     |<--paused time-->|
		// ----*---------------*-----------------*------------*------> time
		//  mBaseTime       mStopTime        startTime     mCurrTime

		if (mStopped)
			return (float)(((mStopTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
		return (float)(((mCurrTime - mPausedTime) - mBaseTime)*mSecondsPerCount);
	};  

	//returns the time between this frame and the last frame in seconds
	inline float DeltaTime() const 
	{
		return (float)mDeltaTime;
	}

	//returns the current tic count in the system for higher resolution performance comparison
	inline long long CurrTics() const
	{
		__int64 tics;
		QueryPerformanceCounter((LARGE_INTEGER*)&tics);
		return tics;
	}

private:
	double mSecondsPerCount;
	double mDeltaTime;
	__int64 mBaseTime;
	__int64 mPausedTime;
	__int64 mStopTime;
	__int64 mPrevTime;
	__int64 mCurrTime;
	bool mStopped;
};