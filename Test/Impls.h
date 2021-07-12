#if !defined(_IMPLS_H_) || defined(_IMPLS_BASE_CLASS)
#include <cstdint>
#ifdef _IMPLS_BASE_CLASS
#undef _BASE_HEADER
#undef _OVERRIDE
#define _BASE_HEADER : public _IMPLS_BASE_CLASS
#define _OVERRIDE override
namespace Virtual {
#else
#undef _BASE_HEADER
#undef _OVERRIDE
#define _BASE_HEADER
#define _OVERRIDE
#endif

#pragma region Impl classes
class DX _BASE_HEADER
{
public:
    float arr[1] = { 1.0f };
    int* ptr;

    constexpr DX()
    {
        ptr = new int;
        *ptr = 0;
    }

    constexpr DX(DX&& d) noexcept
    {
        ptr = d.ptr;
        d.ptr = nullptr;
    }

    constexpr DX(const DX& d) noexcept
    {
        ptr = new int;
        *ptr = *d.ptr;
    }

    DX& operator=(DX&& d) noexcept
    {
        if (ptr)
            delete ptr;
        ptr = d.ptr;
        d.ptr = nullptr;
        return *this;
    }

    DX& operator=(const DX& d) noexcept
    {
        if (!ptr)
            ptr = new int;
        *ptr = *d.ptr;
        return *this;
    }

    virtual ~DX()
    {
        if (ptr)
            delete ptr;
    }

    constexpr float Get() const noexcept _OVERRIDE { return arr[*ptr]; }
};

class Vulk _BASE_HEADER
{
public:
    float arr[2] = { 3.0f, 2.0f };
    int* ptr;

    constexpr Vulk()
    {
        ptr = new int;
        *ptr = 1;
    }

    constexpr Vulk(Vulk&& v) noexcept
    {
        ptr = v.ptr;
        v.ptr = nullptr;
    }

    constexpr Vulk(const Vulk& v) noexcept
    {
        ptr = new int;
        *ptr = *v.ptr;
    }

    Vulk& operator=(Vulk&& v) noexcept
    {
        if (ptr)
            delete ptr;
        ptr = v.ptr;
        v.ptr = nullptr;
        return *this;
    }

    Vulk& operator=(const Vulk& v) noexcept
    {
        if (!ptr)
            ptr = new int;
        *ptr = *v.ptr;
        return *this;
    }

    virtual ~Vulk()
    {
        if (ptr)
            delete ptr;
    }

    constexpr float Get() const noexcept _OVERRIDE { return arr[*ptr]; }
};

class GL _BASE_HEADER
{
public:
    float arr[3] = { 4.0f, 5.0f, 7.0f };
    int* ptr;

    constexpr GL()
    {
        ptr = new int;
        *ptr = 2;
    }

    constexpr GL(GL&& g) noexcept
    {
        ptr = g.ptr;
        g.ptr = nullptr;
    }

    constexpr GL(const GL& g) noexcept
    {
        ptr = new int;
        *ptr = *g.ptr;
    }

    GL& operator=(GL&& g) noexcept
    {
        if (ptr)
            delete ptr;
        ptr = g.ptr;
        g.ptr = nullptr;
        return *this;
    }

    GL& operator=(const GL& g) noexcept
    {
        if (!ptr)
            ptr = new int;
        *ptr = *g.ptr;
        return *this;
    }

    virtual ~GL()
    {
        if (ptr)
            delete ptr;
    }

    constexpr float Get() const noexcept _OVERRIDE { return arr[*ptr]; }
};

#ifdef _IMPLS_BASE_CLASS
}
#endif
#pragma endregion
#endif

#ifndef _IMPLS_H_
#define _IMPLS_H_

typedef uint8_t U8;
enum class Type : U8 { DX11, OpenGL, Vulkan };
enum class PerfType : U8 { None, Function, Global };

Type currentType = Type::DX11;

class Res
{
public:
    Res() = default;
    Res(Res&&) = default;
    Res(const Res&) = default;
    Res& operator=(Res&&) = default;
    Res& operator=(const Res&) = default;
    virtual ~Res() = default;

    virtual float Get() const noexcept = 0;
    virtual constexpr void Switch(Type newType) noexcept {}
};

union Impl
{
    DX dx;
    Vulk vk;
    GL gl;

    constexpr Impl(Type implType) noexcept { Init(implType); }
    ~Impl() { Delete(); }

    Impl(Impl&& i) noexcept
    {
        switch (currentType)
        {
        case Type::DX11:
            new(&dx) DX(std::move(i.dx));
            break;
        case Type::OpenGL:
            new(&gl) GL(std::move(i.gl));
            break;
        case Type::Vulkan:
            new(&vk) Vulk(std::move(i.vk));
            break;
        }
    }

    Impl(const Impl& i) noexcept
    {
        switch (currentType)
        {
        case Type::DX11:
            new(&dx) DX(i.dx);
            break;
        case Type::OpenGL:
            new(&gl) GL(i.gl);
            break;
        case Type::Vulkan:
            new(&vk) Vulk(i.vk);
            break;
        }
    }

    Impl& operator=(Impl&& i) noexcept
    {
        switch (currentType)
        {
        case Type::DX11:
            dx = std::move(i.dx);
            break;
        case Type::OpenGL:
            gl = std::move(i.gl);
            break;
        case Type::Vulkan:
            vk = std::move(i.vk);
            break;
        }
        return *this;
    }

    Impl& operator=(const Impl& i) noexcept
    {
        switch (currentType)
        {
        case Type::DX11:
            dx = i.dx;
            break;
        case Type::OpenGL:
            gl = i.gl;
            break;
        case Type::Vulkan:
            vk = i.vk;
            break;
        }
        return *this;
    }

    constexpr void Init(Type implType) noexcept
    {
        switch (implType)
        {
        case Type::DX11:
            new(&dx) DX;
            break;
        case Type::OpenGL:
            new(&gl) GL;
            break;
        case Type::Vulkan:
            new(&vk) Vulk;
            break;
        }
    }

    constexpr void Delete() noexcept
    {
        switch (currentType)
        {
        case Type::DX11:
            dx.~DX();
            break;
        case Type::OpenGL:
            gl.~GL();
            break;
        case Type::Vulkan:
            vk.~Vulk();
            break;
        }
    }
};
#endif