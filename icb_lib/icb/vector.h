#pragma once

#include <iostream>
#include <initializer_list>
#include <new>
#include <assert.h>

#include <vector>

namespace icb
{
    template <typename T>
    class Vector
    {
    public:
        using SizeType = size_t;
        using ValueType = T;

    private:
        template <typename AccessType = T>
        class BaseIterator
        {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = std::conditional_t<std::is_const_v<AccessType>, const T *, T *>;
            using reference = std::conditional_t<std::is_const_v<AccessType>, const T &, T &>;
            using self_type = BaseIterator<AccessType>;

        public:
            BaseIterator() = default;

            BaseIterator(pointer ptr) noexcept
                : m_Ptr(ptr)
            {
            }

            // Prefix operator++
            self_type &operator++() noexcept
            {
                m_Ptr++;
                return *this;
            }

            // Postfix operator++
            self_type operator++(int) noexcept
            {
                self_type iterator = *this;
                ++(*this);
                return iterator;
            }

            self_type &operator--() noexcept
            {
                m_Ptr--;
                return *this;
            }

            self_type operator--(int) noexcept
            {
                self_type iterator = *this;
                --(*this);
                return iterator;
            }

            self_type &operator+=(SizeType offset) noexcept
            {
                m_Ptr += offset;
                return *this;
            }

            self_type &operator-=(SizeType offset) noexcept
            {
                m_Ptr -= offset;
                return *this;
            }

            self_type operator+(SizeType offset) const noexcept
            {
                self_type result(*this);
                result.m_Ptr += offset;
                return result;
            }

            self_type operator-(SizeType offset) const noexcept
            {
                self_type result(*this);
                result.m_Ptr -= offset;
                return result;
            }

            reference operator[](SizeType index) const noexcept
            {
                return *(m_Ptr + index);
            }

            pointer operator->() const noexcept
            {
                return m_Ptr;
            }

            reference operator*() const noexcept
            {
                return *m_Ptr;
            }

            friend self_type operator+(SizeType offset, const self_type &iter)
            {
                return iter + offset;
            }

            friend difference_type operator+(self_type const &lhs, self_type const &rhs)
            {
                return lhs.m_Ptr + rhs.m_Ptr;
            }

            friend difference_type operator-(self_type const &lhs, self_type const &rhs)
            {
                return lhs.m_Ptr - rhs.m_Ptr;
            }

            bool operator<(self_type const &rhs) const { return m_Ptr < rhs.m_Ptr; }
            bool operator<=(self_type const &rhs) const { return m_Ptr <= rhs.m_Ptr; }
            bool operator>(self_type const &rhs) const { return m_Ptr > rhs.m_Ptr; }
            bool operator>=(self_type const &rhs) const { return m_Ptr >= rhs.m_Ptr; }

            friend bool operator==(const self_type &lhs, const self_type &rhs) noexcept
            {
                return lhs.m_Ptr == rhs.m_Ptr;
            }

            friend bool operator!=(const self_type &lhs, const self_type &rhs) noexcept
            {
                return lhs.m_Ptr != rhs.m_Ptr;
            }

            friend class Vector<T>;

        private:
            pointer m_Ptr;
        };

    public:
        using Iterator = BaseIterator<T>;
        using ConstIterator = BaseIterator<const T>;

    public:
        Vector() = default;

        template <typename InputIterator>
        Vector(InputIterator first, InputIterator last)
        {
            // Reserve once initially, so that PushBack wouldn't need to reallocate incrementally
            Reserve(static_cast<SizeType>(std::distance(first, last)));
            for (; first != last; ++first)
                PushBack(*first);
        }

        Vector(std::initializer_list<ValueType> il)
            : Vector(il.begin(), il.end())
        {
        }

        // copy ctor
        Vector(const Vector &other)
            : Vector(other.cbegin, other.cend())
        {
        }

        // move ctor
        Vector(Vector &&other) noexcept
        {
            std::swap(m_Data, other.m_Data);
            std::swap(m_Size, other.m_Size);
            std::swap(m_Capacity, other.m_Capacity);
        }

        // move assignment
        Vector &operator=(Vector &&other) noexcept
        {
            if (this != &other)
            {
                std::swap(m_Data, other.m_Data);
                std::swap(m_Size, other.m_Size);
                std::swap(m_Capacity, other.m_Capacity);
            }

            return *this;
        }

        // copy assignment
        Vector &operator=(const Vector &other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (other.Size())
            {
                if (other.Capacity() > m_Capacity)
                {
                    ValueType *newData = new ValueType[other.Capacity()];

                    for (SizeType i = 0; i < m_Size; ++i)
                    {
                        newData[i] = other.m_Data[i];
                    }

                    delete[] m_Data;
                    m_Data = newData;
                    m_Capacity = other.Capacity();
                }
                m_Size = other.Size();
            }

            return *this;
        }

        SizeType Size() const noexcept { return m_Size; }
        SizeType Capacity() const noexcept { return m_Capacity; }

        ValueType &operator[](SizeType index) noexcept
        {
            assert(index < m_Size);
            return m_Data[index];
        }
        const ValueType &operator[](SizeType index) const noexcept
        {
            assert(index < m_Size);
            return m_Data[index];
        }

        ValueType *Data() { return m_Data; }
        const ValueType *Data() const { return m_Data; }

        void Reserve(SizeType newCapacity)
        {
            if (newCapacity > m_Capacity)
            {
                reallocate(newCapacity);
            }
        }

        void Resize(SizeType newSize)
        {
            if (newSize == m_Size)
                return;

            if (newSize > m_Size)
            {
                if (newSize > m_Capacity)
                {
                    reallocate(newSize + (newSize + 2) / 2);
                }

                for (SizeType i = m_Size; i < newSize; ++i)
                {
                    new (&m_Data[i]) ValueType();
                }
            }

            if (newSize < m_Size)
            {
                for (SizeType i = m_Size - 1; i >= newSize; --i)
                {
                    m_Data[i].~ValueType();
                }
            }

            m_Size = newSize;
        }

        void Resize(SizeType newSize, const ValueType &value)
        {
            if (newSize == m_Size)
                return;

            if (newSize > m_Size)
            {
                if (newSize > m_Capacity)
                {
                    reallocate(newSize + (newSize + 2) / 2);
                }

                for (SizeType i = m_Size; i < newSize; ++i)
                {
                    m_Data[i] = std::move(value);
                }
            }

            if (newSize < m_Size)
            {
                for (SizeType i = m_Size - 1; i >= newSize; --i)
                {
                    m_Data[i].~ValueType();
                }
            }

            m_Size = newSize;
        }

        [[deprecated("not implemented")]] void Insert([[maybe_unused]] Iterator position, [[maybe_unused]] ValueType value)
        {
        }

        Iterator Erase(Iterator position)
        {
            (*position).~ValueType();

            if (position + 1 != end())
            {
                std::copy(position + 1, end(), position);
            }
            --m_Size;
            return position;
        }

        void Clear()
        {
            for (SizeType i = 0; i < m_Size; ++i)
            {
                m_Data[i].~ValueType();
            }

            m_Size = 0;
        }

        void PushBack(const ValueType &value)
        {
            if (m_Size >= m_Capacity)
                reallocate(m_Capacity + (m_Capacity + 2) / 2);

            m_Data[m_Size] = value;
            ++m_Size;
        }

        void PushBack(ValueType &&value)
        {
            if (m_Size >= m_Capacity)
                reallocate(m_Capacity + (m_Capacity + 2) / 2);

            m_Data[m_Size] = std::move(value);
            ++m_Size;
        }

        template <typename... Args>
        ValueType &EmplaceBack(Args &&...args)
        {
            if (m_Size >= m_Capacity)
                reallocate(m_Capacity + (m_Capacity + 2) / 2);

            new (&m_Data[m_Size]) ValueType(std::forward<Args>(args)...);
            // m_Data[m_Size] = ValueType(std::forward<Args>(args)...);
            /*
             * Instead of constructing ValueType in the stack frame of EmplaceBack() and then moving it to m_Data,
             * we can use placement new, where we provide a valid address inside m_Data where the construction
             * would take place.
             */
            ++m_Size;
            return m_Data[m_Size];
        }

        void PopBack()
        {
            if (m_Size > 0)
            {
                --m_Size;
                m_Data[m_Size].~ValueType();
            }
        }

        ValueType &At(SizeType index)
        {
            if (index >= m_Size)
            {
                throw std::out_of_range("Vector::at: index out of range");
            }
            return m_Data[index];
        }

        const ValueType &At(SizeType index) const
        {
            if (index >= m_Size)
            {
                throw std::out_of_range("Vector::at: index out of range");
            }
            return m_Data[index];
        }

        Iterator begin()
        {
            return Iterator(m_Data);
        }

        Iterator end()
        {
            return Iterator(m_Data + m_Size);
        }

        ConstIterator cbegin()
        {
            return ConstIterator(m_Data);
        }

        ConstIterator cend()
        {
            return ConstIterator(m_Data + m_Size);
        }

        ~Vector()
        {
            /*
             * delete[] would call destructors, which would cause a bug.
             * If we had a more complex type, which maybe had some memory allocated on the heap
             * that had to be deleted in their destructors,
             * any time we PopBack or Clear, the destructor would be called and release that memory.
             * If delete[] gets called here, it would also call that same destructor again.
             * Which would try to free memory which has been already freed.
             *
             * This happens because delete[] will not stop at our m_Size like our Clear function does.
             */
            Clear();                                                   // Clear will call all the destructors
            ::operator delete(m_Data, m_Capacity * sizeof(ValueType)); // this delete will not call any destructors.
        }

    private:
        void reallocate(SizeType newCapacity)
        {
            /*
             * Using memcpy will not call copy constructors and will be a shallow copy. It is fine for primitive types like int.
             * Hence, use a for loop.
             */

            /*
             * We don't need to call constructors here, because we want uninitialized data that will be
             * pushed back to instead of creating default objects waiting to be overwritten.
             */
            ValueType *newData = (ValueType *)::operator new(newCapacity * sizeof(ValueType));

            // for downsizing, otherwise would cause an overflow.
            // could argue that this is not the responsibility of this function though.
            if (newCapacity < m_Size)
                m_Size = newCapacity;

            for (SizeType i = 0; i < m_Size; ++i)
            {
                new (&newData[i]) ValueType(std::move(m_Data[i]));
                // newData[i] = std::move(m_Data[i]);
                /*
                 * Can't use the assigment operator, either copy or move.
                 * Assignment operator requires newData[i] object to actually exist,
                 * but we never called a constructor on it due to the ::operator new above.
                 *
                 */
            }

            /*
             * Everything that has been moved to newData is safe, and we can call the destructors on m_Data here.
             *
             */
            for (SizeType i = 0; i < m_Size; ++i)
            {
                m_Data[i].~ValueType();
            }
            if (m_Capacity)
                ::operator delete(m_Data, m_Capacity * sizeof(ValueType));

            m_Data = newData;
            m_Capacity = newCapacity;
        }

    private:
        ValueType *m_Data = nullptr;
        SizeType m_Size = 0;
        SizeType m_Capacity = 0;
    };
} // namespace icb
