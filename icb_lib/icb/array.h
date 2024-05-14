#pragma once
#include <iostream>
#include <assert.h>

namespace icb
{
    template <typename T, size_t SIZE>
    class Array
    {
    public:
        [[deprecated("Not implemented, undelete this ctor")]] Array() = delete;
        ~Array() = default;

        constexpr size_t Size() const
        {
            return SIZE;
        }

    private:
        T m_data[SIZE];
    };
} // namespace icb
