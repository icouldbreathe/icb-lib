/**
 * @file array.h
 * @author icouldbreathe
 * @brief Not implemented
 * @version 0.1
 * @date 2024-05-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once
#include <assert.h>
#include <iostream>

namespace icb
{
template <typename T, size_t SIZE> class Array
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
