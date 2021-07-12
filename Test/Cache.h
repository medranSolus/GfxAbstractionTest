#pragma once
#include <vector>
#include "Impls.h"
#include "Perf.h"

namespace Cache
{
    float GetData(U64 key) noexcept;

    class Base : public Res
    {
    public:
        U64 key;

        constexpr Base(U64 key) noexcept : key(key) {}
        Base(Base&&) = default;
        Base(const Base&) = default;
        Base& operator=(Base&&) = default;
        Base& operator=(const Base&) = default;
        ~Base() = default;

        float Get() const noexcept override { return GetData(key); }
    };

    std::unordered_map<U64, Impl> cache;

    float GetData(U64 key) noexcept
    {
        switch (currentType)
        {
        default:
        case Type::DX11:
            return cache.at(key).dx.Get();
        case Type::OpenGL:
            return cache.at(key).gl.Get();
        case Type::Vulkan:
            return cache.at(key).vk.Get();
        }
    }

    template<PerfType perfType, bool resBase, typename T>
    float Test(U64 size)
    {
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_START("CG");
        std::vector<T> codex;
        codex.reserve(size);
        cache.reserve(size);
        currentType = Type::DX11;
        for (U64 i = 0; i < size; ++i)
        {
            if constexpr (resBase)
                codex.emplace_back(new Base(i));
            else
                codex.emplace_back(i);
            cache.emplace(i, currentType);
        }
        float acc = 0.0f;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "CDXR" : "CDX");
            float val;
            if constexpr (resBase)
                val = ptr->Get();
            else
                val = ptr.Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        for (U64 i = 0; i < size; ++i)
        {
            cache.at(i).Delete();
            cache.at(i).Init(Type::Vulkan);
        }
        currentType = Type::Vulkan;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "CVKR" : "CVK");
            float val;
            if constexpr (resBase)
                val = ptr->Get();
            else
                val = ptr.Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        for (U64 i = 0; i < size; ++i)
        {
            cache.at(i).Delete();
            cache.at(i).Init(Type::OpenGL);
        }
        currentType = Type::OpenGL;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "CGLR" : "CGL");
            float val;
            if constexpr (resBase)
                val = ptr->Get();
            else
                val = ptr.Get();
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_STOP();
            acc += val;
        }
        if constexpr (resBase)
            for (U64 i = 0; i < size; ++i)
                delete codex.at(i);
        codex.clear();
        cache.clear();
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_STOP();
        return acc;
    }
}