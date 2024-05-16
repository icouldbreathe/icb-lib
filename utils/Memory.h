#pragma once

#define ALLOC_OVERRIDE 1

#include <iostream>

class MemoryProfiler
{
  public:
    static MemoryProfiler &Get()
    {
        static MemoryProfiler instance;
        return instance;
    }

    void Alloc(size_t size, void *ptr)
    {
        m_allocMem += size;
        if (!m_silent)
            std::cout << "\033[1;32m" << size << "\tbytes allocated at: \t" << ptr << "\033[0m" << std::endl;
    }

    void DeAlloc(void *ptr, size_t size)
    {
        m_deallocMem += size;
        if (!m_silent)
            std::cout << "\033[1;31m" << size << "\tbytes deallocated at: \t" << ptr << "\033[0m" << std::endl;
    }

    void DeAlloc(size_t size)
    {
        m_deallocMem += size;
        if (!m_silent)
            std::cout << "\033[1;31m" << size << "\tbytes deallocated."
                      << "\033[0m" << std::endl;
    }

    void DeAlloc(void *ptr)
    {
        if (!m_silent)
            std::cout << "\033[1;31mUnsized deallocated at: \t" << ptr << "\033[0m" << std::endl;
    }

    void PrintMemory() const
    {
        std::cout << "Total memory allocated: " << m_allocMem << " bytes\n";
        std::cout << "Total memory deallocated: " << m_deallocMem << " bytes\n";
    }

    void Reset()
    {
        m_allocMem = 0;
        m_deallocMem = 0;
    }

    void Silent(bool enable)
    {
        m_silent = enable;
    }

  private:
    MemoryProfiler() = default;

  private:
    uint32_t m_allocMem = 0;
    uint32_t m_deallocMem = 0;
    bool m_silent = true;
};

#if ALLOC_OVERRIDE == 1
void *operator new[](size_t size)
{
    void *ptr = malloc(size);
    MemoryProfiler::Get().Alloc(size, ptr);
    return ptr;
}

void *operator new(size_t size)
{
    void *ptr = malloc(size);
    MemoryProfiler::Get().Alloc(size, ptr);
    return ptr;
}

void operator delete(void *p, size_t size) noexcept
{
    MemoryProfiler::Get().DeAlloc(p, size);
    free(p);
}

void operator delete(void *p) noexcept
{
    MemoryProfiler::Get().DeAlloc(p);
    free(p);
}

void operator delete[](void *p, size_t size) noexcept
{
    MemoryProfiler::Get().DeAlloc(p, size);
    free(p);
}

void operator delete[](void *p) noexcept
{
    MemoryProfiler::Get().DeAlloc(p);
    free(p);
}
#endif