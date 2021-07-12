#include <iostream>
#include <fstream>
#include <intrin.h>
#pragma intrinsic()
#include "Cache.h"
#include "Proxy.h"
#include "Virtual.h"

constexpr U64 DATA_COUNT = 9;

void RunTests();
void MergeTests();

int main()
{
    RunTests();
    // Before merging tests data gather output files with name pattern, ex: 1.txt, 2.txt, 3.txt, etc.
    // Then place them in directory Tests and adjust variable DATA_COUNT
    //MergeTests();
}

void MergeTests()
{
    constexpr U64 SECTION_COUNT = 18;

    std::pair<std::string, ZE::Perf::Data>** data = new std::pair<std::string, ZE::Perf::Data>*[DATA_COUNT];
    ZE::Perf::DataHeader* headers = new ZE::Perf::DataHeader[DATA_COUNT];
    for (U64 i = 0; i < DATA_COUNT; ++i)
    {
        data[i] = new std::pair<std::string, ZE::Perf::Data>[SECTION_COUNT];
        std::ifstream fin("Tests/" + std::to_string(i + 1) + ".txt");
        if (!fin.good())
            continue;
        fin >> headers[i].memoryCount >> headers[i].coreCount;
        std::getline(fin, headers[i].cpuName);
        for (U64 j = 0; j < SECTION_COUNT; ++j)
            fin >> data[i][j].second.tests >> data[i][j].second.avg >> data[i][j].second.m2k >> data[i][j].first;
        fin.close();
    }

    std::ofstream fout("Results.txt");
    if (fout.good())
    {
        constexpr const char* HEADERS[SECTION_COUNT] =
        {
            "CDX", "CVK", "CGL",
            "CDXR", "CVKR", "CGLR",
            "VDX", "VVK", "VGL",
            "PDX", "PVK", "PGL",
            "PDXR", "PVKR", "PGLR",
            "CG", "VG", "PG"
        };
        constexpr U64 MEANS_COUNT = 5;
        constexpr const char* MEANS_HEADERS[MEANS_COUNT] =
        {
            "Cache",
            "Cache Res",
            "Virtual",
            "Proxy",
            "Proxy Res"
        };
        long double** means = new long double*[DATA_COUNT];

        fout << "--------- TESTS ---------\nTests count: " << data[0][0].second.tests << " (or other number in brackets)";
        for (U64 i = 0; i < SECTION_COUNT - 3; ++i)
            fout << "\t\t\t" << HEADERS[i];
        for (U64 i = SECTION_COUNT - 3; i < SECTION_COUNT; ++i)
        {
            for (U64 j = 0; j < SECTION_COUNT; ++j)s
            {
                if (data[0][j].first == HEADERS[i])
                {
                    fout << "\t\t\t" << HEADERS[i] << " (" << data[0][j].second.tests << ")\t";
                    break;
                }
            }
        }
        for (U64 i = 0; i < DATA_COUNT; ++i)
        {
            means[i] = new long double[MEANS_COUNT];
            for (U64 j = 0; j < MEANS_COUNT; ++j)
                means[i][j] = 0.0L;

            fout << std::endl << std::to_string(i + 1) << ". " << headers[i].memoryCount << "MB\t" << headers[i].coreCount << "x\t" << headers[i].cpuName << "\t";
            for (U64 j = 0, m = 0; j < SECTION_COUNT; ++j)
            {
                for (U64 k = 0; k < SECTION_COUNT; ++k)
                {
                    if (data[i][k].first == HEADERS[j])
                    {
                        if (m < MEANS_COUNT)
                        {
                            means[i][m] += data[i][k].second.avg;
                            if (j % 3 == 2)
                                means[i][m++] /= 3.0L;
                        }
                        fout << ' ' << data[i][k].second.avg << " (" << data[i][k].second.m2k << ")\t";
                        break;
                    }
                }
            }
        }
        fout << "\n\n-------- RESULTS --------\n\t";
        for (U64 i = 0; i < MEANS_COUNT; ++i)
            fout << MEANS_HEADERS[i] << "\t\t";
        for (U64 i = 0; i < DATA_COUNT; ++i)
        {
            fout << std::endl << std::to_string(i + 1) << ".\t";
            for (U64 j = 0; j < MEANS_COUNT; ++j)
                fout << means[i][j] << "\t\t";
        }

        long double cr = 0.0L, pr = 0.0L, pv = 0.0L, pc = 0.0L, vc = 0.0L;
        for (U64 i = 0; i < DATA_COUNT; ++i)
        {
            cr += means[i][0] / means[i][1];
            pr += means[i][3] / means[i][4];
            pv += means[i][3] / means[i][2];
            pc += means[i][3] / means[i][0];
            vc += means[i][2] / means[i][0];
        }
        fout << "\n\nCache\t/ Cache Res\t= " << cr / DATA_COUNT;
        fout << "\nProxy\t/ Proxy Res\t= " << pr / DATA_COUNT;
        fout << "\n\nProxy\t/ Virtual\t= " << pv / DATA_COUNT;
        fout << "\nProxy\t/ Cache\t\t= " << pc / DATA_COUNT;
        fout << "\nVirtual / Cache\t\t= " << vc / DATA_COUNT;

        for (U64 i = 0; i < DATA_COUNT; ++i)
            delete[] means[i];
        delete[] means;
        fout.close();
    }

    for (U64 i = 0; i < DATA_COUNT; ++i)
        delete[] data[i];
    delete[] data;
    delete[] headers;
}
//Tests count: 100000000 (or other number in brackets)		    CDX					CVK					CGL					CDXR				CVKR				CGLR				VDX					VVK					VGL					PDX					PVK					PGL					PDXR				PVKR				PGLR				CG (20)						VG (10)						PG (20)

void RunTests()
{
#ifdef _DEBUG
    constexpr U64 SIZE = 100'000;
#else
    constexpr U64 SIZE = 10'000'000;
#endif
    constexpr U64 TEST_RUNS = 10;
    constexpr PerfType GLOBAL = PerfType::Global;
    constexpr PerfType FUNCTION = PerfType::Function;

    for (U64 i = 0; i < TEST_RUNS; ++i)
    {
        std::cout << "Proxy res: "
            << Proxy::Test<FUNCTION, true, Res*>(SIZE) << std::endl;
        std::cout << "Proxy res global: "
            << Proxy::Test<GLOBAL, true, Res*>(SIZE) << std::endl;
        std::cout << "Proxy: "
            << Proxy::Test<FUNCTION, false, Proxy::Base>(SIZE) << std::endl;
        std::cout << "Proxy global: "
            << Proxy::Test<GLOBAL, false, Proxy::Base>(SIZE) << std::endl;
        std::cout << "Virtual: "
            << Virtual::Test<FUNCTION>(SIZE) << std::endl;
        std::cout << "Virtual global: "
            << Virtual::Test<GLOBAL>(SIZE) << std::endl;
        std::cout << "Cache res: "
            << Cache::Test<FUNCTION, true, Res*>(SIZE) << std::endl;
        std::cout << "Cache res global: "
            << Cache::Test<GLOBAL, true, Res*>(SIZE) << std::endl;
        std::cout << "Cache: "
            << Cache::Test<FUNCTION, false, Cache::Base>(SIZE) << std::endl;
        std::cout << "Cache global: "
            << Cache::Test<GLOBAL, false, Cache::Base>(SIZE) << std::endl;
    }
}