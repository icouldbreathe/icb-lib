#pragma once

#include <new>
#include <iterator>
#include <assert.h>

namespace icb
{
    template <typename T>
    class LinkedList
    {
    public:
        using ValueType = T;
        using SizeType = size_t;
        using Reference = T &;
        using Pointer = T *;

    private:
        struct NodeLink
        {
            NodeLink *prev;
            NodeLink *next;

            NodeLink() = default;

            NodeLink(NodeLink *prev, NodeLink *next)
                : prev(prev), next(next)
            {
            }
        };

        struct Node : NodeLink
        {
            ValueType value;

            Node() = default;

            Node(const ValueType &value)
                : value(value)
            {
            }
        };

    public:
        template <typename AccessType = T>
        class Iterator
        {
        public:
            using iterator_category = std::bidirectional_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = T;
            using pointer = std::conditional_t<std::is_const_v<AccessType>, const T *, T *>;
            using reference = std::conditional_t<std::is_const_v<AccessType>, const T &, T &>;
            using node_ptr = std::conditional_t<std::is_const_v<AccessType>, const NodeLink *, NodeLink *>;
            using self_type = Iterator<AccessType>;

        public:
            Iterator() = default;

            explicit Iterator(node_ptr ptr)
                : m_Ptr(ptr)
            {
            }

            // move ctor
            Iterator(Iterator &&other) noexcept
                : m_Ptr(other.m_Ptr)
            {
                other.m_Ptr = nullptr;
            }
            
            // move assignment
            self_type &operator=(Iterator &&other) noexcept
            {
                if (this != &other)
                {
                    m_Ptr = other.m_Ptr;
                    other.m_Ptr = nullptr;
                }
                return *this;
            }

            // prefix ++
            self_type &operator++() noexcept
            {
                m_Ptr = m_Ptr->next;
                return *this;
            }

            // postfix ++
            self_type operator++(int) noexcept
            {
                self_type previous = *this;
                m_Ptr = m_Ptr->next;
                return previous;
            }

            // prefix --
            self_type &operator--() noexcept
            {
                m_Ptr = m_Ptr->prev;
                return *this;
            }

            // postfix --
            self_type operator--(int) noexcept
            {
                self_type previous = *this;
                m_Ptr = m_Ptr->prev;
                return previous;
            }

            reference operator*() const
            {
                assert(m_Ptr && "LinkedList::Iterator::operator* nullptr dereference");
                return asNode(m_Ptr)->value;
            }

            friend bool operator==(const self_type &x, const self_type &y) noexcept
            {
                return x.m_Ptr == y.m_Ptr;
            }

            friend bool operator!=(const self_type &x, const self_type &y) noexcept
            {
                return x.m_Ptr != y.m_Ptr;
            }

        private:
            node_ptr m_Ptr;
        };

    public:
        using ConstIterator = Iterator<const T>;

    public:
        LinkedList() = default;
        ~LinkedList()
        {
            while (m_End.next != &m_End)
            {
                Node *tmp = asNode(m_End.next);
                m_End.next = m_End.next->next;
                asNode(tmp)->value.~ValueType();
                ::operator delete(asNode(tmp), sizeof(Node));
            }
        }

        template <typename InputIterator>
        LinkedList(InputIterator first, InputIterator last)
        {
            for (; first != last; ++first)
                PushBack(*first);
        }

        LinkedList(std::initializer_list<ValueType> init)
            : LinkedList(init.begin(), init.end())
        {
        }

        // copy ctor
        LinkedList(const LinkedList &other)
            : LinkedList(other.cbegin(), other.cend())
        {
        }

        // move ctor
        LinkedList(LinkedList &&other) noexcept
        {
            if (this != &other)
            {
                m_End = other.m_End;
                m_Size = other.m_Size;

                m_End.next->prev = &m_End;
                m_End.prev->next = &m_End;

                other.m_End.prev = &other.m_End;
                other.m_End.next = &other.m_End;

                other.m_Size = 0;
            }
        }

        // copy assignment
        LinkedList &operator=(const LinkedList &other)
        {
            if (this != &other)
            {
                Clear();
                for (auto it = other.cbegin(); it != other.cend(); ++it)
                {
                    PushBack(*it);
                }
            }
            return *this;
        }

        // move assignment
        LinkedList &operator=(LinkedList &&other) noexcept
        {
            if (this != &other)
            {
                Clear();
                m_End = other.m_End;
                m_Size = other.m_Size;

                m_End.next->prev = &m_End;
                m_End.prev->next = &m_End;

                other.m_End.next = &other.m_End;
                other.m_End.prev = &other.m_End;
                other.m_Size = 0;
            }
            return *this;
        }

        // copy
        void PushBack(const ValueType &value)
        {
            Node *newNode = new Node();
            newNode->value = value;
            newNode->next = &m_End;
            if (m_End.prev)
            {
                newNode->prev = m_End.prev;
                m_End.prev->next = newNode;
            }
            else
            {
                newNode->prev = &m_End;
                m_End.next = newNode;
            }
            m_End.prev = newNode;
            ++m_Size;
        }

        // move
        void PushBack(ValueType &&value)
        {
            Node *newNode = new Node();
            newNode->value = std::move(value);
            newNode->next = &m_End;
            if (m_End.prev)
            {
                newNode->prev = m_End.prev;
                m_End.prev->next = newNode;
            }
            else
            {
                newNode->prev = &m_End;
                m_End.next = newNode;
            }
            m_End.prev = newNode;
            ++m_Size;
        }

        template <typename... Args>
        void EmplaceBack(Args &&...args)
        {
            Node *newNode = new Node();
            new (&newNode->value) ValueType(std::forward<Args>(args)...);
            newNode->next = &m_End;
            if (m_End.prev)
            {
                newNode->prev = m_End.prev;
                m_End.prev->next = newNode;
            }
            else
            {
                newNode->prev = &m_End;
                m_End.next = newNode;
            }
            m_End.prev = newNode;
            ++m_Size;
        }

        // copy
        void PushFront(const ValueType &value)
        {
            Node *newNode = new Node();
            newNode->value = value;
            newNode->next = m_End.next;
            newNode->prev = &m_End;
            if (m_Size)
            {
                m_End.next->prev = newNode;
            }
            else
            {
                m_End.prev = newNode;
            }
            m_End.next = newNode;
            ++m_Size;
        }

        // move
        void PushFront(ValueType &&value)
        {
            Node *newNode = new Node();
            newNode->value = std::move(value);
            newNode->next = m_End.next;
            newNode->prev = &m_End;
            if (m_Size)
            {
                m_End.next->prev = newNode;
            }
            else
            {
                m_End.prev = newNode;
            }
            m_End.next = newNode;
            ++m_Size;
        }

        template <typename... Args>
        void EmplaceFront(Args &&...args)
        {
            Node *newNode = new Node();
            new (&newNode->value) ValueType(std::forward<Args>(args)...);
            newNode->next = m_End.next;
            newNode->prev = &m_End;
            if (m_Size)
            {
                m_End.next->prev = newNode;
            }
            else
            {
                m_End.prev = newNode;
            }
            m_End.next = newNode;
            ++m_Size;
        }

        void PopFront()
        {
            assert(m_Size && "LinkedList::PopFront empty list");
            Node *second = asNode(m_End.next->next);
            asNode(m_End.next)->value.~ValueType();
            ::operator delete(asNode(m_End.next), sizeof(Node));
            m_End.next = second;
            m_End.next->prev = &m_End;
            --m_Size;
        }

        void PopBack()
        {
            assert(m_Size && "LinkedList::PopBack empty list");
            Node *previous = asNode(m_End.prev->prev);
            previous->next = &m_End;
            asNode(m_End.prev)->value.~ValueType();
            ::operator delete(asNode(m_End.prev), sizeof(Node));
            m_End.prev = previous;
            --m_Size;
        }

        void Clear()
        {
            if (Empty())
                return;
            while (m_End.next != &m_End)
            {
                Node *tmp = asNode(m_End.next);
                m_End.next = m_End.next->next;
                asNode(tmp)->value.~ValueType();
                ::operator delete(asNode(tmp), sizeof(Node));
            }

            m_End.prev = &m_End;
            m_End.next = &m_End;
            m_Size = 0;
        }

        Reference Front() const noexcept
        {
            assert(!Empty() && "LinkedList::Front empty list");
            return asNode(m_End.next)->value;
        }

        Reference Back() const noexcept
        {
            assert(!Empty() && "LinkedList:Back empty list");
            return asNode(m_End.prev)->value;
        }

        SizeType Size() const noexcept
        {
            return m_Size;
        }

        bool Empty() const noexcept
        {
            return !m_Size;
        }

        Iterator<T> begin() noexcept
        {
            return Iterator(m_End.next);
        }

        Iterator<T> end() noexcept
        {
            return Iterator(&m_End);
        }

        ConstIterator cbegin() const noexcept
        {
            return ConstIterator(m_End.next);
        }

        ConstIterator cend() const noexcept
        {
            return ConstIterator(&m_End);
        }

    private:
        static inline Node *asNode(NodeLink *node) noexcept
        {
            return static_cast<Node *>(node);
        }

        static inline NodeLink *asNodeLink(Node *node) noexcept
        {
            return static_cast<NodeLink *>(node);
        }

        static inline const Node *asNode(const NodeLink *node) noexcept
        {
            return static_cast<const Node *>(node);
        }

        static inline const NodeLink *asNodeLink(const Node *node) noexcept
        {
            return static_cast<const NodeLink *>(node);
        }

    private:
        NodeLink m_End{&m_End, &m_End};
        SizeType m_Size = 0;
    };

} // namespace icb
