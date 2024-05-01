#pragma once
#include <iostream>
#include <assert.h>

namespace icb
{
    template <typename T, size_t SIZE>
    class Array
    {
    public:
        Array() = default;
        ~Array() = default;

        Array(std::initializer_list<T> &&list)
        {
            int i = 0;
            for (auto value : list)
            {
                m_Data[i] = value;
                ++i;
            }
        }

        T &operator[](size_t index)
        {
            assert(!(index >= SIZE));
            return m_Data[index];
        }

        const T &operator[](size_t index) const
        {
            assert(!(index >= SIZE));
            return m_Data[index];
        }

        constexpr size_t Size() const
        {
            return SIZE;
        }

    private:
        T m_Data[SIZE];
    };
} // namespace icb
