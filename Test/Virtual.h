#pragma once
#include <vector>
#include "Perf.h"

namespace Virtual
{
    class Base : public Res
    {
    public:
        Base() = default;
        Base(Base&&) = default;
        Base(const Base&) = default;
        Base& operator=(Base&&) = default;
        Base& operator=(const Base&) = default;
        virtual ~Base() = default;
    };
}

#define _IMPLS_BASE_CLASS Virtual::Base
#include "Impls.h"
#undef _IMPLS_BASE_CLASS

namespace Virtual
{
    template<PerfType perfType>
    float Test(U64 size) noexcept
    {
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_START("VG");
        std::vector<Res*> codex;
        codex.reserve(size);
        for (U64 i = 0; i < size; ++i)
            codex.push_back(new DX);
        float acc = 0.0f;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START("VDX");
            const float val = ptr->Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        for (U64 i = 0; i < size; ++i)
        {
            delete codex.at(i);
            codex.at(i) = new Vulk;
        }
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START("VVK");
            const float val = ptr->Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        for (U64 i = 0; i < size; ++i)
        {
            delete codex.at(i);
            codex.at(i) = new GL;
        }
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START("VGL");
            const float val = ptr->Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        for (U64 i = 0; i < size; ++i)
            delete codex.at(i);
        codex.clear();
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_STOP();
        return acc;
    }
}