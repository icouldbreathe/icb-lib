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
                : m_ptr(ptr)
            {
            }

            /**
             * @brief SFINAE copy ctor for implicit conversion between const Iterator and Iterator
             * 
             * @tparam WasAccessType The source type
             * @tparam std::enable_if_t<std::is_const_v<AccessType> && !std::is_const_v<WasAccessType>> SFINAE
             * @param other 
             */
            template<typename WasAccessType, class = std::enable_if_t<std::is_const_v<AccessType> && !std::is_const_v<WasAccessType>>>
            BaseIterator(const BaseIterator<WasAccessType> &other) noexcept : m_ptr(other.m_ptr) {}

            // Prefix operator++
            self_type &operator++() noexcept
            {
                m_ptr++;
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
                m_ptr--;
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
                m_ptr += offset;
                return *this;
            }

            self_type &operator-=(SizeType offset) noexcept
            {
                m_ptr -= offset;
                return *this;
            }

            self_type operator+(SizeType offset) const noexcept
            {
                self_type result(*this);
                result.m_ptr += offset;
                return result;
            }

            self_type operator-(SizeType offset) const noexcept
            {
                self_type result(*this);
                result.m_ptr -= offset;
                return result;
            }

            reference operator[](SizeType index) const noexcept
            {
                return *(m_ptr + index);
            }

            pointer operator->() const noexcept
            {
                return m_ptr;
            }

            reference operator*() const noexcept
            {
                return *m_ptr;
            }

            friend self_type operator+(SizeType offset, const self_type &iter)
            {
                return iter + offset;
            }

            friend difference_type operator+(self_type const &lhs, self_type const &rhs)
            {
                return lhs.m_ptr + rhs.m_ptr;
            }

            friend difference_type operator-(self_type const &lhs, self_type const &rhs)
            {
                return lhs.m_ptr - rhs.m_ptr;
            }

            bool operator<(self_type const &rhs) const { return m_ptr < rhs.m_ptr; }
            bool operator<=(self_type const &rhs) const { return m_ptr <= rhs.m_ptr; }
            bool operator>(self_type const &rhs) const { return m_ptr > rhs.m_ptr; }
            bool operator>=(self_type const &rhs) const { return m_ptr >= rhs.m_ptr; }

            friend bool operator==(const self_type &lhs, const self_type &rhs) noexcept
            {
                return lhs.m_ptr == rhs.m_ptr;
            }

            friend bool operator!=(const self_type &lhs, const self_type &rhs) noexcept
            {
                return lhs.m_ptr != rhs.m_ptr;
            }

            friend class Vector<T>;

        private:
            pointer m_ptr;
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
            std::swap(m_data, other.m_data);
            std::swap(m_size, other.m_size);
            std::swap(m_capacity, other.m_capacity);
        }

        // move assignment
        Vector &operator=(Vector &&other) noexcept
        {
            if (this != &other)
            {
                std::swap(m_data, other.m_data);
                std::swap(m_size, other.m_size);
                std::swap(m_capacity, other.m_capacity);
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
                if (other.Capacity() > m_capacity)
                {
                    ValueType *newData = new ValueType[other.Capacity()];

                    for (SizeType i = 0; i < m_size; ++i)
                    {
                        newData[i] = other.m_data[i];
                    }

                    delete[] m_data;
                    m_data = newData;
                    m_capacity = other.Capacity();
                }
                m_size = other.Size();
            }

            return *this;
        }

        SizeType Size() const noexcept { return m_size; }
        SizeType Capacity() const noexcept { return m_capacity; }

        ValueType &operator[](SizeType index) noexcept
        {
            assert(index < m_size);
            return m_data[index];
        }
        const ValueType &operator[](SizeType index) const noexcept
        {
            assert(index < m_size);
            return m_data[index];
        }

        ValueType *Data() { return m_data; }
        const ValueType *Data() const { return m_data; }

        void Reserve(SizeType newCapacity)
        {
            if (newCapacity > m_capacity)
            {
                reallocate(newCapacity);
            }
        }

        void Resize(SizeType newSize)
        {
            if (newSize == m_size)
                return;

            if (newSize > m_size)
            {
                if (newSize > m_capacity)
                {
                    reallocate(newSize + (newSize + 2) / 2);
                }

                for (SizeType i = m_size; i < newSize; ++i)
                {
                    new (&m_data[i]) ValueType();
                }
            }

            if (newSize < m_size)
            {
                for (SizeType i = m_size - 1; i >= newSize; --i)
                {
                    m_data[i].~ValueType();
                }
            }

            m_size = newSize;
        }

        void Resize(SizeType newSize, const ValueType &value)
        {
            if (newSize == m_size)
                return;

            if (newSize > m_size)
            {
                if (newSize > m_capacity)
                {
                    reallocate(newSize + (newSize + 2) / 2);
                }

                for (SizeType i = m_size; i < newSize; ++i)
                {
                    m_data[i] = std::move(value);
                }
            }

            if (newSize < m_size)
            {
                for (SizeType i = m_size - 1; i >= newSize; --i)
                {
                    m_data[i].~ValueType();
                }
            }

            m_size = newSize;
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
            --m_size;
            return position;
        }

        void Clear()
        {
            for (SizeType i = 0; i < m_size; ++i)
            {
                m_data[i].~ValueType();
            }

            m_size = 0;
        }

        void PushBack(const ValueType &value)
        {
            if (m_size >= m_capacity)
                reallocate(m_capacity + (m_capacity + 2) / 2);

            m_data[m_size] = value;
            ++m_size;
        }

        void PushBack(ValueType &&value)
        {
            if (m_size >= m_capacity)
                reallocate(m_capacity + (m_capacity + 2) / 2);

            m_data[m_size] = std::move(value);
            ++m_size;
        }

        template <typename... Args>
        ValueType &EmplaceBack(Args &&...args)
        {
            if (m_size >= m_capacity)
                reallocate(m_capacity + (m_capacity + 2) / 2);

            new (&m_data[m_size]) ValueType(std::forward<Args>(args)...);
            // m_data[m_size] = ValueType(std::forward<Args>(args)...);
            /*
             * Instead of constructing ValueType in the stack frame of EmplaceBack() and then moving it to m_data,
             * we can use placement new, where we provide a valid address inside m_data where the construction
             * would take place.
             */
            ++m_size;
            return m_data[m_size];
        }

        void PopBack()
        {
            if (m_size > 0)
            {
                --m_size;
                m_data[m_size].~ValueType();
            }
        }

        ValueType &At(SizeType index)
        {
            if (index >= m_size)
            {
                throw std::out_of_range("Vector::at: index out of range");
            }
            return m_data[index];
        }

        const ValueType &At(SizeType index) const
        {
            if (index >= m_size)
            {
                throw std::out_of_range("Vector::at: index out of range");
            }
            return m_data[index];
        }

        Iterator begin()
        {
            return Iterator(m_data);
        }

        Iterator end()
        {
            return Iterator(m_data + m_size);
        }

        ConstIterator cbegin()
        {
            return ConstIterator(m_data);
        }

        ConstIterator cend()
        {
            return ConstIterator(m_data + m_size);
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
             * This happens because delete[] will not stop at our m_size like our Clear function does.
             */
            Clear();                                                   // Clear will call all the destructors
            ::operator delete(m_data, m_capacity * sizeof(ValueType)); // this delete will not call any destructors.
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
            if (newCapacity < m_size)
                m_size = newCapacity;

            for (SizeType i = 0; i < m_size; ++i)
            {
                new (&newData[i]) ValueType(std::move(m_data[i]));
                // newData[i] = std::move(m_data[i]);
                /*
                 * Can't use the assigment operator, either copy or move.
                 * Assignment operator requires newData[i] object to actually exist,
                 * but we never called a constructor on it due to the ::operator new above.
                 *
                 */
            }

            /*
             * Everything that has been moved to newData is safe, and we can call the destructors on m_data here.
             *
             */
            for (SizeType i = 0; i < m_size; ++i)
            {
                m_data[i].~ValueType();
            }
            if (m_capacity)
                ::operator delete(m_data, m_capacity * sizeof(ValueType));

            m_data = newData;
            m_capacity = newCapacity;
        }

    private:
        ValueType *m_data = nullptr;
        SizeType m_size = 0;
        SizeType m_capacity = 0;
    };
} // namespace icb
