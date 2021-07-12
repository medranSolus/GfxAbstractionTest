#include "Perf.h"
#include <fstream>
#include <Windows.h>
#include <intrin.h>
#pragma intrinsic(__faststorefence, __rdtsc, __cpuid)

namespace ZE
{
	void Perf::Save()
	{
		std::ofstream fout(LOG_FILE, std::ios_base::app);
		if (!fout.good())
			return;

		int CPUInfo[4] = { -1 };
		unsigned int nExIds, i = 0;
		char CPUBrandString[0x40];
		// Get the information associated with each extended ID
		__cpuid(CPUInfo, 0x80000000);
		nExIds = CPUInfo[0];
		for (i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string
			if (i == 0x80000002)
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000003)
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			else if (i == 0x80000004)
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
		}
		SYSTEM_INFO sysInfo;
		GetSystemInfo(&sysInfo);
		MEMORYSTATUSEX statex;
		statex.dwLength = sizeof(statex);
		GlobalMemoryStatusEx(&statex);
		fout << (statex.ullTotalPhys >> 20) << ' ' << sysInfo.dwNumberOfProcessors << CPUBrandString << std::fixed << std::endl;

		for (auto& x : data)
		{
			if (x.second.tests > 0)
				x.second.m2k = sqrt(x.second.m2k / x.second.tests);
			fout << x.second.tests << ' ' << x.second.avg << ' ' << x.second.m2k << ' ' << x.first << std::endl;
		}
		data.clear();
		fout.close();
	}

	Perf::~Perf()
	{
		if (data.size() > 0)
			Save();
	}

	void Perf::Start(const std::string& sectionTag) noexcept
	{
		std::string tag = "";
		if (data.find(sectionTag) == data.end())
			data.emplace(sectionTag + tag, Data(0, 0.0L, 0.0L));
		lastTag = sectionTag;
		__faststorefence();
		stamp = __rdtsc();
	}

	void Perf::Stop() noexcept
	{
		U64 end = __rdtsc();
		__faststorefence();
		end -= stamp;
		const long double delta = end - data.at(lastTag).avg;
		data.at(lastTag).avg += delta / ++data.at(lastTag).tests;
		data.at(lastTag).m2k += delta * (end - data.at(lastTag).avg);
		stamp = 0;
		lastTag = "";
	}
}