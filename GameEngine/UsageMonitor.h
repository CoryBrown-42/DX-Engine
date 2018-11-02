#pragma once

#include <Windows.h>
#include <Pdh.h>
#include <Psapi.h>
#include <string>

/*
UsageMonitor gets the amount of ram and cpu usage (%) that the game is using.
Like AlgorithmTimer, this is just for debug configuration
*/
class UsageMonitor
{
public:
	//Constructor
	inline UsageMonitor()
	{
		PdhOpenQuery(NULL,0,&cpuhandle);
		PdhAddCounter(cpuhandle,TEXT("\\Processor(_Total)\\% processor time"), 0, &counterhandle);
		process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
		GetProcessMemoryInfo(process, &mem, sizeof(mem)); 
	}

	//Update refreshes the ram/cpu usage data, preferrably not every frame.
	inline void Update()
	{
		PdhCollectQueryData(cpuhandle);
		PdhGetFormattedCounterValue(counterhandle,PDH_FMT_LONG,NULL,&value);
		cpuusage = value.longValue;			
		GetProcessMemoryInfo(process, &mem, sizeof(mem));	
	} 

	//Returns the percentage of cpu the program is using
	inline std::wstring CPUPercent()
	{
		return std::to_wstring(cpuusage);
	}

	//Returns the amount of RAM in MB the program is using
	inline std::wstring WorkingSetSizeMB()
	{
		return std::to_wstring(mem.WorkingSetSize/1024.0f/1024.0f);
	}

	//Returns the amount of Virtual Memory in MB the program is using
	inline std::wstring PagefileUsageMB()
	{
		return std::to_wstring(mem.PagefileUsage/1024.0f/1024.0f);
	}

protected:  
	HQUERY cpuhandle;
	HCOUNTER counterhandle; 
	long cpuusage;
	PDH_FMT_COUNTERVALUE value;
	HANDLE process;
	PROCESS_MEMORY_COUNTERS mem;	 
};