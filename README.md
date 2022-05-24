# Graphics API Abstraction Layer Architectures Test
Simple program testing possible implementations of graphics API abstraction layer.

## Tested architectures
Test was run for 3 main architectures, named **Cache**, **Virtual** and **Proxy**, with 3 classes representing implementations for different graphics APIs.
Additionally Cache and Proxy are also tested with or without base class **Res** that would represent common resource class.

Implementations in Cache and Proxy architectures are hold inside `Impl` union, allowing to save space on each possible implementation at cost of being always the size of the biggest implementation object.

- **Cache**
  - One cache based on `std::unordered_map` with unique key and union of possible implementations.
  - Function `GetData(U64)` returning implementation data based on global variable representing current active API.
  - Class `Base` holding key to the actual implementation and accessing it's data via `GetData(U64)` function.
  - Switching API based on iterating over cache, calling destructor on every implementation and initializing with new one.
- **Virtual**
  - Class `Base` from which each implementation is derived.
  - Data returned via normal method overloading.
  - Switching API based on deleting old implementation objects and creating new ones.
- **Proxy**
  - Class `Base` with implementations as member data and initializing them based on passed current API type.
  - Accessing data via simple function that decides which API is active basing on state of global variable.
  - Switching API based on call to `Switch(Type)` method with new implementation type passed in.

## Test cases
Each architecture was measured in terms of accessing implementation data and overall tests time. Test algorithm was run `10` times on `10 000 000` objects. Algorithm:

1. Initialize architecture data.
2. Access and add implementation data for all created objects.
3. If this is last architecture go to point 5.
4. Swap current implementations with next architecture and go to point 2.
5. Delete all architecture data.

This test was performed for all present architectures (tests prefixed with first letter of architecture) and measured in terms of access speed (tests named with used API)
and overall test speed (tests named **G**) of 3 mocked implementations of graphics APIs:
`DirectX` (tests named **DX**), `Vulkan` (tests named **VK**) and `OpenGL` (tests named **GL**).
Additionally for Cache and Proxy in configurations where `Res` class is present all tests were given sufix **R** (global measurement includes both configurations).
Example: `CDXR` - Cache DirectX with Resource as base class, `VVK` - Virtual Vulkan, `PG` - Proxy Global.

## Results
Results of tests were gathered on few available machines with 64 bit Windows 10 and are presented in [Results.txt](Results.txt).
Conclusions that comes from performed tests are:

- **Cache** is almost 3 times slower from other architectures.
- Using `Res` class as base for container costs 6-7% of performance on **Cache** and **Proxy** architectures.
- Memory usage of **Proxy** is slightly higher than **Virtual** (but can vary if some implementations are heavy on data).
- **Cache** uses over 3 times more memory  than **Proxy** in current implementation.
- **Proxy** is almost 2% faster than **Virtual**.

According to current data, it is possibly better to follow architecture represented by `Proxy` since it reduces number of times the memory is allocated and slightly speeds up
access time on cost of little more space occupied per object. Another pro might be better usage of cache which could speed everything up even more since **Proxy**
avoids trashing CPU instruction pipeline with accessing virtual methods via traversing vtable for each of them.

---
Copyright (c) 2021 Marek Machliński
