#pragma once

#include <iostream>
#include <initializer_list>
#include <new>
#include <assert.h>

namespace icb
{
    template <typename Vector>
    class VectorIterator
    {
    public:
        using ValueType = typename Vector::ValueType;
        using PointerType = ValueType *;
        using ReferenceType = ValueType &;

    public:
        VectorIterator(PointerType ptr)
            : m_Ptr(ptr)
        {
        }

        // Prefix operator++
        VectorIterator &operator++()
        {
            m_Ptr++;
            return *this;
        }

        // Postfix operator++
        VectorIterator operator++(int)
        {
            VectorIterator iterator = *this;
            ++(*this);
            return iterator;
        }

        VectorIterator &operator--()
        {
            m_Ptr--;
            return *this;
        }

        VectorIterator operator--(int)
        {
            VectorIterator iterator = *this;
            --(*this);
            return iterator;
        }

        ReferenceType operator[](size_t index)
        {
            return *(m_Ptr + index);
        }

        PointerType operator->()
        {
            return m_Ptr;
        }

        ReferenceType operator*()
        {
            return *m_Ptr;
        }

        bool operator==(const VectorIterator &other) const
        {
            return m_Ptr == other.m_Ptr;
        }

        bool operator!=(const VectorIterator &other) const
        {
            return m_Ptr != other.m_Ptr;
        }

    private:
        PointerType m_Ptr;
    };

    template <typename T>
    class Vector
    {
    public:
        using ValueType = T;
        using Iterator = VectorIterator<Vector<T>>;

    public:
        Vector() = default;

        // move assignment
        Vector &operator=(Vector<T> &&other) noexcept
        {
            m_Data = other.m_Data;
            m_Size = other.m_Size;
            m_Capacity = other.m_Capacity;

            other.m_Data = nullptr;
            other.m_Size = 0;
            other.m_Capacity = 0;

            std::cout << "Moved" << std::endl;

            return *this;
        }

        // copy assignment
        Vector &operator=(const Vector<T> &other)
        {
            if (this == &other)
            {
                return *this;
            }

            if (other.Size())
            {
                if (other.Capacity() > m_Capacity)
                {
                    T *newData = new T[other.Capacity()];

                    for (size_t i = 0; i < m_Size; ++i)
                    {
                        newData[i] = other.m_Data[i];
                    }

                    delete[] m_Data;
                    m_Data = newData;
                    m_Capacity = other.Capacity();
                }
                m_Size = other.Size();
            }

            std::cout << "Copied" << std::endl;

            return *this;
        }

        // initializer list
        Vector(std::initializer_list<T> init)
            : m_Data(nullptr), m_Size(0), m_Capacity(0)
        {
            assign(init);
        }

        Vector &operator=(std::initializer_list<T> init)
        {
            assign(init);
            return *this;
        }

        void assign(const std::initializer_list<T> il)
        {
            if (il.size() > m_Capacity)
            {
                ReAlloc(il.size());
            }
            std::copy(il.begin(), il.end(), m_Data);
            m_Capacity = il.size();
            m_Size = il.size();
        }

        size_t Size() const { return m_Size; }
        size_t Capacity() const { return m_Capacity; }

        T &operator[](size_t index)
        {
            assert(index < m_Size);
            return m_Data[index];
        }
        const T &operator[](size_t index) const
        {
            assert(index < m_Size);
            return m_Data[index];
        }

        T *Data() { return m_Data; }
        const T *Data() const { return m_Data; }

        void Reserve(size_t newCapacity)
        {
            if (newCapacity > m_Capacity)
            {
                ReAlloc(newCapacity);
            }
        }

        void Clear()
        {
            for (size_t i = 0; i < m_Size; ++i)
            {
                m_Data[i].~T();
            }

            m_Size = 0;
        }

        void PushBack(const T &value)
        {
            if (m_Size >= m_Capacity)
                ReAlloc(m_Capacity + (m_Capacity + 2) / 2);

            m_Data[m_Size] = value;
            ++m_Size;
        }

        void PushBack(T &&value)
        {
            if (m_Size >= m_Capacity)
                ReAlloc(m_Capacity + (m_Capacity + 2) / 2);

            m_Data[m_Size] = std::move(value);
            ++m_Size;
        }

        template <typename... Args>
        T &EmplaceBack(Args &&...args)
        {
            if (m_Size >= m_Capacity)
                ReAlloc(m_Capacity + (m_Capacity + 2) / 2);

            new (&m_Data[m_Size]) T(std::forward<Args>(args)...);
            // m_Data[m_Size] = T(std::forward<Args>(args)...);
            /*
             * Instead of constructing T in the stack frame of EmplaceBack() and then moving it to m_Data,
             * we can use placement new, where we provide a valid address inside m_Data where the construction
             * would take place.
             */
            return m_Data[m_Size++];
        }

        void PopBack()
        {
            if (m_Size > 0)
            {
                --m_Size;
                // calls the destructor
                m_Data[m_Size].~T();
            }
        }

        T &at(size_t index)
        {
            if (index >= m_Size)
            {
                throw std::out_of_range("Vector::at: index out of range");
            }
            return m_Data[index];
        }

        const T &at(size_t index) const
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
            Clear();                                           // Clear will call all the destructors
            ::operator delete(m_Data, m_Capacity * sizeof(T)); // this delete will not call any destructors.
        }

    private:
        void ReAlloc(size_t newCapacity)
        {
            /*
             * Using memcpy will not call copy constructors and will be a shallow copy. It is fine for primitive types like int.
             * Hence, use a for loop.
             */

            /*
             * We don't need to call constructors here, because we want uninitialized data that will be
             * pushed back to instead of creating default objects waiting to be overwritten.
             */
            T *newData = (T *)::operator new(newCapacity * sizeof(T));

            // for downsizing, otherwise would cause an overflow.
            // could argue that this is not the responsibility of this function though.
            if (newCapacity < m_Size)
                m_Size = newCapacity;

            for (size_t i = 0; i < m_Size; ++i)
            {
                new (&newData[i]) T(std::move(m_Data[i]));
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
             * ::operator delete() will not call a destructor.
             * Same for ::operator new, which will not call the constructor.
             */
            for (size_t i = 0; i < m_Size; ++i)
            {
                m_Data[i].~T();
            }
            ::operator delete(m_Data, m_Capacity * sizeof(T));

            m_Data = newData;
            m_Capacity = newCapacity;
        }

    private:
        T *m_Data = nullptr;
        size_t m_Size = 0;
        size_t m_Capacity = 0;
    };
}
