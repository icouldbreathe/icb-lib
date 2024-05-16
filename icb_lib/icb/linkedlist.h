/**
 * @file linkedlist.h
 * @author Dovydas Ciomenas (icouldbreathe@icloud.com)
 * @brief Doubly-linked Linked List with sentry node
 * @version 0.1
 * @date 2024-05-16
 *
 * @copyright Copyright (c) 2024
 *
 */

#pragma once

#include <assert.h>
#include <iterator>
#include <new>
#include <type_traits>

namespace icb
{
template <typename T> class LinkedList
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

        NodeLink(NodeLink *prev, NodeLink *next) : prev(prev), next(next)
        {
        }
    };

    struct Node : NodeLink
    {
        ValueType value;

        Node() = default;

        Node(const ValueType &value) : value(value)
        {
        }
    };

  private:
    template <typename AccessType = T> class BaseIterator
    {
      public:
        using iterator_category = std::bidirectional_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = T;
        using pointer = std::conditional_t<std::is_const_v<AccessType>, const T *, T *>;
        using reference = std::conditional_t<std::is_const_v<AccessType>, const T &, T &>;
        using node_ptr = std::conditional_t<std::is_const_v<AccessType>, const NodeLink *, NodeLink *>;
        using self_type = BaseIterator<AccessType>;

      public:
        BaseIterator() = default;

        explicit BaseIterator(node_ptr ptr) : m_ptr(ptr)
        {
        }

        /**
         * @brief SFINAE copy ctor for implicit conversion between ConstIterator and Iterator
         *
         * @tparam WasAccessType The source type
         * @tparam std::enable_if_t<std::is_const_v<AccessType> && !std::is_const_v<WasAccessType>> SFINAE
         * @param other
         */
        template <typename WasAccessType,
                  class = std::enable_if_t<std::is_const_v<AccessType> && !std::is_const_v<WasAccessType>>>
        BaseIterator(const BaseIterator<WasAccessType> &other) noexcept : m_ptr(other.m_ptr)
        {
        }

        // prefix ++
        self_type &operator++() noexcept
        {
            m_ptr = m_ptr->next;
            return *this;
        }

        // postfix ++
        self_type operator++(int) noexcept
        {
            self_type previous = *this;
            m_ptr = m_ptr->next;
            return previous;
        }

        // prefix --
        self_type &operator--() noexcept
        {
            m_ptr = m_ptr->prev;
            return *this;
        }

        // postfix --
        self_type operator--(int) noexcept
        {
            self_type previous = *this;
            m_ptr = m_ptr->prev;
            return previous;
        }

        reference operator*() const
        {
            assert(m_ptr && "LinkedList::Iterator::operator* nullptr dereference");
            return asNode(m_ptr)->value;
        }

        node_ptr operator->() const
        {
            return asNode(m_ptr);
        }

        friend bool operator==(const self_type &x, const self_type &y) noexcept
        {
            return x.m_ptr == y.m_ptr;
        }

        friend bool operator!=(const self_type &x, const self_type &y) noexcept
        {
            return x.m_ptr != y.m_ptr;
        }

        friend class LinkedList;

      private:
        node_ptr m_ptr;
    };

  public:
    using Iterator = BaseIterator<T>;
    using ConstIterator = BaseIterator<const T>;

  public:
    LinkedList() = default;
    ~LinkedList()
    {
        while (m_end.next != &m_end)
        {
            Node *tmp = asNode(m_end.next);
            m_end.next = m_end.next->next;
            asNode(tmp)->value.~ValueType();
            ::operator delete(asNode(tmp), sizeof(Node));
        }
    }

    template <typename InputIterator> LinkedList(InputIterator first, InputIterator last)
    {
        for (; first != last; ++first)
            PushBack(*first);
    }

    LinkedList(std::initializer_list<ValueType> init) : LinkedList(init.begin(), init.end())
    {
    }

    // copy ctor
    LinkedList(const LinkedList &other) : LinkedList(other.cbegin(), other.cend())
    {
    }

    // move ctor
    LinkedList(LinkedList &&other) noexcept
    {
        if (this != &other)
        {
            m_end = other.m_end;
            m_size = other.m_size;

            m_end.next->prev = &m_end;
            m_end.prev->next = &m_end;

            other.m_end.prev = &other.m_end;
            other.m_end.next = &other.m_end;

            other.m_size = 0;
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
            m_end = other.m_end;
            m_size = other.m_size;

            m_end.next->prev = &m_end;
            m_end.prev->next = &m_end;

            other.m_end.next = &other.m_end;
            other.m_end.prev = &other.m_end;
            other.m_size = 0;
        }
        return *this;
    }

    // copy
    void PushBack(const ValueType &value)
    {
        Node *newNode = new Node();
        newNode->value = value;
        newNode->next = &m_end;
        if (m_size)
        {
            newNode->prev = m_end.prev;
            m_end.prev->next = newNode;
        }
        else
        {
            newNode->prev = &m_end;
            m_end.next = newNode;
        }
        m_end.prev = newNode;
        ++m_size;
    }

    // move
    void PushBack(ValueType &&value)
    {
        Node *newNode = new Node();
        newNode->value = std::move(value);
        newNode->next = &m_end;
        if (m_size)
        {
            newNode->prev = m_end.prev;
            m_end.prev->next = newNode;
        }
        else
        {
            newNode->prev = &m_end;
            m_end.next = newNode;
        }
        m_end.prev = newNode;
        ++m_size;
    }

    template <typename... Args> void EmplaceBack(Args &&...args)
    {
        Node *newNode = new Node();
        new (&newNode->value) ValueType(std::forward<Args>(args)...);
        newNode->next = &m_end;
        if (m_size)
        {
            newNode->prev = m_end.prev;
            m_end.prev->next = newNode;
        }
        else
        {
            newNode->prev = &m_end;
            m_end.next = newNode;
        }
        m_end.prev = newNode;
        ++m_size;
    }

    // copy
    void PushFront(const ValueType &value)
    {
        Node *newNode = new Node();
        newNode->value = value;
        newNode->next = m_end.next;
        newNode->prev = &m_end;
        if (m_size)
        {
            m_end.next->prev = newNode;
        }
        else
        {
            m_end.prev = newNode;
        }
        m_end.next = newNode;
        ++m_size;
    }

    // move
    void PushFront(ValueType &&value)
    {
        Node *newNode = new Node();
        newNode->value = std::move(value);
        newNode->next = m_end.next;
        newNode->prev = &m_end;
        if (m_size)
        {
            m_end.next->prev = newNode;
        }
        else
        {
            m_end.prev = newNode;
        }
        m_end.next = newNode;
        ++m_size;
    }

    template <typename... Args> void EmplaceFront(Args &&...args)
    {
        Node *newNode = new Node();
        new (&newNode->value) ValueType(std::forward<Args>(args)...);
        newNode->next = m_end.next;
        newNode->prev = &m_end;
        if (m_size)
        {
            m_end.next->prev = newNode;
        }
        else
        {
            m_end.prev = newNode;
        }
        m_end.next = newNode;
        ++m_size;
    }

    void Splice(Iterator position, LinkedList &other, Iterator it)
    {
        assert(getNodeLink(position) && getNodeLink(it) && "LinkedList::Splice invalid iterators");
        assert(!other.Empty() && "LinkedList::Splice attempt to splice an empty list");
        assert(it != other.end() && "LinkedList::Splice splicing the end node is undefined behavior");

        NodeLink *prev = getNodeLink(std::prev(position));
        NodeLink *next = getNodeLink(position);

        prev->next = getNodeLink(it);
        next->prev = getNodeLink(it);

        getNodeLink(std::prev(it))->next = getNodeLink(it)->next;
        getNodeLink(std::next(it))->prev = getNodeLink(it)->prev;

        getNodeLink(it)->prev = prev;
        getNodeLink(it)->next = next;

        --other.m_size;
        ++m_size;
    }

    void PopFront()
    {
        assert(!Empty() && "LinkedList::PopFront empty list");
        Node *second = asNode(m_end.next->next);
        asNode(m_end.next)->value.~ValueType();
        ::operator delete(asNode(m_end.next), sizeof(Node));
        m_end.next = second;
        m_end.next->prev = &m_end;
        --m_size;
    }

    void PopBack()
    {
        assert(!Empty() && "LinkedList::PopBack empty list");
        Node *previous = asNode(m_end.prev->prev);
        previous->next = &m_end;
        asNode(m_end.prev)->value.~ValueType();
        ::operator delete(asNode(m_end.prev), sizeof(Node));
        m_end.prev = previous;
        --m_size;
    }

    void Clear()
    {
        if (Empty())
            return;
        while (m_end.next != &m_end)
        {
            Node *tmp = asNode(m_end.next);
            m_end.next = m_end.next->next;
            asNode(tmp)->value.~ValueType();
            ::operator delete(asNode(tmp), sizeof(Node));
        }

        m_end.prev = &m_end;
        m_end.next = &m_end;
        m_size = 0;
    }

    Iterator Erase(Iterator position)
    {
        assert(getNodeLink(position) && "LinkedList::Erase invalid iterator");
        assert(!Empty() && "LinkedList::Erase empty list");

        if (position == begin())
        {
            PopFront();
            return begin();
        }

        NodeLink *next = getNodeLink(std::next(position));
        getNodeLink(std::prev(position))->next = next;
        next->prev = getNodeLink(position)->prev;

        delete asNode(getNodeLink(position));

        --m_size;

        return Iterator(next);
    }

    Reference Front() const noexcept
    {
        assert(!Empty() && "LinkedList::Front empty list");
        return asNode(m_end.next)->value;
    }

    Reference Back() const noexcept
    {
        assert(!Empty() && "LinkedList:Back empty list");
        return asNode(m_end.prev)->value;
    }

    SizeType Size() const noexcept
    {
        return m_size;
    }

    bool Empty() const noexcept
    {
        return !m_size;
    }

    Iterator begin() noexcept
    {
        return Iterator(m_end.next);
    }

    Iterator end() noexcept
    {
        return Iterator(&m_end);
    }

    ConstIterator cbegin() const noexcept
    {
        return ConstIterator(m_end.next);
    }

    ConstIterator cend() const noexcept
    {
        return ConstIterator(&m_end);
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

    static inline NodeLink *getNodeLink(Iterator it) noexcept
    {
        return it.m_ptr;
    }

    static inline const NodeLink *getNodeLink(ConstIterator it) noexcept
    {
        return it.m_ptr;
    }

  private:
    NodeLink m_end{&m_end, &m_end};
    SizeType m_size = 0;
};

} // namespace icb
