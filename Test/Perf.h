#pragma once
#include <string>
#include <unordered_map>
#include <cstdint>

typedef uint64_t U64;
typedef uint32_t U32;

namespace ZE
{
	class Perf final
	{
	public:
		struct Data
		{
			U64 tests;
			long double avg;
			long double m2k;
		};
		struct DataHeader
		{
			U64 memoryCount;
			U32 coreCount;
			std::string cpuName;
		};

	private:
		static constexpr const char* LOG_FILE = "perf_log.txt";

		std::unordered_map<std::string, Data> data;
		std::string lastTag = "";
		U64 stamp = 0;

		void Save();

		Perf() = default;

	public:
		Perf(Perf&&) = default;
		Perf(const Perf&) = default;
		Perf& operator=(Perf&&) = default;
		Perf& operator=(const Perf&) = default;
		~Perf();

		static Perf& Get() noexcept { static Perf perf; return perf; }

		void Start(const std::string& sectionTag) noexcept;
		void Stop() noexcept;
	};
}

#define ZE_PERF_START(sectionTag) ZE::Perf::Get().Start(sectionTag)
#define ZE_PERF_STOP() ZE::Perf::Get().Stop()