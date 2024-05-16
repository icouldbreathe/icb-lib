/**
 * @file hash_table.h
 * @author Dovydas Ciomenas (icouldbreathe@icloud.com)
 * @brief Unordered hash table (map) with separate chaining
 * @version 0.1
 * @date 2024-05-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <functional>
#include <initializer_list>
#include <optional>

#include <utility> // std::pair

#include "linkedlist.h"
#include "vector.h"

#define HT_INIT_CAPACITY 11

namespace icb
{
template <typename Key, typename Value> class HashTable
{
  public:
    using SizeType = size_t;
    using Pair = std::pair<Key, Value>;
    using Chain = icb::LinkedList<Pair>;
    using Table = icb::Vector<Chain>;

  public:
    HashTable(const SizeType &capacity = HT_INIT_CAPACITY) : m_capacity(capacity)
    {
        m_table.Reserve(capacity);
        m_table.Resize(capacity);
    }

    ~HashTable()
    {
        Clear();
    }

    HashTable(std::initializer_list<Pair> il, const SizeType &capacity = HT_INIT_CAPACITY) : m_capacity(capacity)
    {
        m_table.Reserve(capacity);
        m_table.Resize(capacity);

        for (const auto &[key, value] : il)
        {
            Insert(key, value);
        }
    }

    // copy
    void Insert(const Key &key, const Value &value)
    {
        SizeType index = hash(key);

        if (!Find(key).has_value())
        {
            m_table[index].EmplaceBack(key, value);
            ++m_elements;
        }
    }

    // move
    void Insert(Key &&key, Value &&value)
    {
        SizeType index = hash(key);

        if (!Find(key).has_value())
        {
            m_table[index].EmplaceBack(std::move(key), std::move(value));
            ++m_elements;
        }
    }

    std::optional<Value> Find(const Key &key)
    {
        SizeType index = hash(key);

        for (auto &[existingKey, value] : m_table[index])
        {
            if (key == existingKey)
            {
                return value;
            }
        }
        return std::nullopt;
    }

    void Erase(const Key &key)
    {
        SizeType index = hash(key);

        Chain &chain = m_table[index];

        for (auto it = chain.begin(); it != chain.end(); ++it)
        {
            auto [existingKey, value] = *it;
            if (existingKey == key)
            {
                chain.Erase(it);
                --m_elements;
                return;
            }
        }
    }

    void Clear()
    {
        for (auto &chain : m_table)
        {
            chain.Clear();
        }

        m_table.Clear();
    }

    void Rehash(const SizeType &newCapacity)
    {
        assert(newCapacity > 0 && "HashTable::Resize attempt to resize to <=0");

        Table newTable;
        newTable.Reserve(newCapacity);
        newTable.Resize(newCapacity);

        for (SizeType i = 0; i < m_capacity; ++i)
        {
            Chain &current = m_table[i];
            for (auto it = current.begin(); it != current.end(); ++it)
            {
                const auto &[existingKey, value] = *it;

                SizeType newIndex = hash(existingKey, newCapacity);
                Chain &newChain = newTable[newIndex];
                typename Chain::Iterator tempIt = it;
                --it;
                newChain.Splice(newChain.end(), current, tempIt);
            }
        }

        m_table = std::move(newTable);
        m_capacity = newCapacity;
    }

    bool Empty() const
    {
        return !m_elements;
    }

    SizeType Size() const
    {
        return m_elements;
    }

  private:
    SizeType hash(const Key &key) noexcept
    {
        std::hash<Key> hasher;
        return hasher(key) % m_capacity;
    }

    SizeType hash(const Key &key, const SizeType &capacity) noexcept
    {
        std::hash<Key> hasher;
        return hasher(key) % capacity;
    }

  private:
    Table m_table;
    SizeType m_capacity = 0;
    SizeType m_elements = 0;
};

} // namespace icb