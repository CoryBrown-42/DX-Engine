#pragma once

#include <string>

/*
This class is for precisely timing how many ticks a block of code takes.
I intend it only for DEBUG mode (enclosed in #if defined(_DEBUG) ... #endif)
Pass the current ticks into Start and Stop around a block of code, then whenever you call Average
it will return how much time on average that operation took sine the last Average()
This is necessary since the differences at the level of precision are too big to use it every frame.
*/
class AlgorithmTimer
{
public:
	//Constructor
	inline AlgorithmTimer(const std::string &name = "") 
		: name(name), 
		startTime(0), 
		totalTime(0), 
		frames(0)
	{ 

	}

	//Start justs store the current ticks when its called
	inline void Start(long long t)
	{ 
		startTime = t; 
	}

	//Stop records how long passed since Start was called and that is one test (assumed to be every frame is why i called it frame)
	inline void Stop(long long t)
	{ 
		frames+=1;
		totalTime += t - startTime; 
	}

	//For outputting what this timer is timing at runtime
	inline std::string Name() const
	{ 
		return name; 
	} 

	//Average calculates how long it took on average to execute and resets the timer.
	inline double long AverageAndClear()
	{ 
		if(frames == 0)
			return 0;
		double avg = double(totalTime)/frames; 
		frames = 0; 
		totalTime = 0;
		return avg;
	}
	
	inline double long Average()
	{
		if (frames == 0)
			return 0;
		double avg = double(totalTime) / frames; 
		return avg;
	}

protected:
	std::string name;
	long long startTime, totalTime;
	int frames;
};