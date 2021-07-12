#pragma once
#include <vector>
#include "Impls.h"
#include "Perf.h"

namespace Proxy
{
    class Base : public Res
    {
        Impl impl;

    public:
        constexpr Base(Type implType) noexcept : impl(implType) {}
        Base(Base&&) = default;
        Base(const Base&) = default;
        Base& operator=(Base&&) = default;
        Base& operator=(const Base&) = default;
        ~Base() = default;

        constexpr float Get() const noexcept override
        {
            switch (currentType)
            {
            default:
            case Type::DX11:
                return impl.dx.Get();
            case Type::OpenGL:
                return impl.gl.Get();
            case Type::Vulkan:
                return impl.vk.Get();
            }
        }

        constexpr void Switch(Type newType) noexcept override
        {
            impl.Delete();
            impl.Init(newType);
        }
    };

    template<PerfType perfType, bool resBase, typename T>
    float Test(U64 size) noexcept
    {
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_START("PG");
        std::vector<T> codex;
        codex.reserve(size);
        currentType = Type::DX11;
        for (U64 i = 0; i < size; ++i)
        {
            if constexpr (resBase)
                codex.emplace_back(new Base(currentType));
            else
                codex.emplace_back(currentType);
        }
        float acc = 0.0f;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "PDXR" : "PDX");
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
            if constexpr (resBase)
                codex.at(i)->Switch(Type::Vulkan);
            else
                codex.at(i).Switch(Type::Vulkan);
        }
        currentType = Type::Vulkan;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "PVKR" : "PVK");
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
            if constexpr (resBase)
                codex.at(i)->Switch(Type::OpenGL);
            else
                codex.at(i).Switch(Type::OpenGL);
        }
        currentType = Type::OpenGL;
        for (auto& ptr : codex)
        {
            if constexpr (perfType == PerfType::Function)
                ZE_PERF_START(resBase ? "PGLR" : "PGL");
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
        if constexpr (perfType == PerfType::Global)
            ZE_PERF_STOP();
        return acc;
    }
}