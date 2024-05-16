/**
 * @file AVLSet.h
 * @author Dovydas Ciomenas (icouldbreathe@icloud.com)
 * @brief AVL tree set
 * @version 0.1
 * @date 2024-05-16
 *
 * @copyright Copyright (c) 2024
 *
 */
#pragma once

#include <concepts>
#include <functional>
#include <iomanip>
#include <iostream>
#include <type_traits>

namespace icb
{
template <typename A, typename B>
concept Comparable = requires(A a, B b) {
    { a < b } -> std::convertible_to<bool>;
    { a > b } -> std::convertible_to<bool>;
    { a == b } -> std::convertible_to<bool>;
    { a != b } -> std::convertible_to<bool>;
};

template <typename T> class AVLSet
{
  public:
    using SizeType = size_t;
    using ValueType = T;

  private:
    using HeightType = int32_t;

  private:
    struct Node
    {
        ValueType data;
        Node *left;
        Node *right;
        HeightType height;

        Node() : Node(ValueType{})
        {
        }

        Node(const ValueType &data) : data(data), left(nullptr), right(nullptr), height(1)
        {
        }

        Node(const ValueType &data, Node *left, Node *right) : data(data), left(left), right(right), height(1)
        {
        }
    };

  public:
    enum TraversalOrder
    {
        INORDER,
        PREORDER,
        POSTORDER
    };

  public:
    AVLSet() = default;
    ~AVLSet()
    {
        deleteSubtree(m_root);
    }

    // copy ctor
    explicit AVLSet(const AVLSet<T> &other) noexcept
    {
        if (other.m_root == nullptr)
        {
            return;
        }

        m_root = copySubtree(other.m_root);
    }

    // move ctor
    explicit AVLSet(AVLSet<T> &&other) noexcept : m_root(other.m_root)
    {
        other.m_root = nullptr;
    }

    // copy assignment
    AVLSet &operator=(const AVLSet<T> &other) noexcept
    {
        if (other.m_root == nullptr)
        {
            m_root = nullptr;
            return *this;
        }

        if (this == &other)
        {
            return *this;
        }

        deleteSubtree(m_root);

        m_root = copySubtree(other.m_root);

        return *this;
    }

    // move assignment
    AVLSet &operator=(AVLSet &&other) noexcept
    {
        if (this == &other)
        {
            return *this;
        }

        m_root = other.m_root;
        other.m_root = nullptr;

        return *this;
    }

  public:
    // copy
    void Insert(const ValueType &data)
    {
        m_root = insertAt(m_root, data);
    }
    // move
    void Insert(ValueType &&data)
    {
        m_root = insertAt(m_root, std::move(data));
    }

    [[deprecated("Not implemented")]] void Delete([[maybe_unused]] const ValueType &data)
    {
    }

    bool Contains(const ValueType &data) const noexcept
    {
        return containsAt(m_root, data);
    }

    void ForEachNode(TraversalOrder order, std::function<void(const ValueType &)> fn) const noexcept
    {
        switch (order)
        {
        case INORDER:
            inorderTraversal(m_root, fn);
            break;
        case PREORDER:
            preorderTraversal(m_root, fn);
            break;
        case POSTORDER:
            postorderTraversal(m_root, fn);
            break;
        }
    }

    void PrintTree()
    {
        printSubtree(m_root, 0);
    }

  private:
    void inorderTraversal(Node *node, std::function<void(const ValueType &)> fn) const noexcept
    {
        if (node == nullptr)
        {
            return;
        }

        inorderTraversal(node->left, fn);

        fn(node->data);

        inorderTraversal(node->right, fn);
    }

    void preorderTraversal(Node *node, std::function<void(const ValueType &)> fn) const noexcept
    {
        if (node == nullptr)
        {
            return;
        }

        fn(node->data);

        preorderTraversal(node->left, fn);

        preorderTraversal(node->right, fn);
    }

    void postorderTraversal(Node *node, std::function<void(const ValueType &)> fn) const noexcept
    {
        if (node == nullptr)
        {
            return;
        }

        postorderTraversal(node->left, fn);

        postorderTraversal(node->right, fn);

        fn(node->data);
    }

    template <typename InsertType>
        requires Comparable<InsertType, ValueType>
    Node *insertAt(Node *node, InsertType &&data)
    {
        static_assert(std::is_same_v<std::decay_t<InsertType>, ValueType>,
                      "InsertType must be the same type as ValueType");

        if (node == nullptr)
        {
            return new Node(std::forward<InsertType>(data));
        }

        if (data < node->data)
            node->left = insertAt(node->left, std::forward<InsertType>(data));
        else if (data > node->data)
            node->right = insertAt(node->right, std::forward<InsertType>(data));
        else
            return node;

        node->height = 1 + std::max(height(node->left), height(node->right));
        int balanceFactor = getBalance(node);

        if (balanceFactor > 1)
        {
            if (data < node->left->data)
            {
                return rotateRight(node);
            }
            else if (data > node->left->data)
            {
                node->left = rotateLeft(node->left);
                return rotateRight(node);
            }
        }
        if (balanceFactor < -1)
        {
            if (data > node->right->data)
            {
                return rotateLeft(node);
            }
            else if (data < node->right->data)
            {
                node->right = rotateRight(node->right);
                return rotateLeft(node);
            }
        }
        return node;
    }

    bool containsAt(Node *node, const ValueType &data) const noexcept
    {
        if (node == nullptr)
        {
            return false;
        }

        if (data == node->data)
        {
            return true;
        }
        else if (data < node->data)
        {
            return containsAt(node->left, data);
        }
        else if (data > node->data)
        {
            return containsAt(node->right, data);
        }

        return false;
    }

    void deleteSubtree(Node *node)
    {
        if (node == nullptr)
        {
            return;
        }

        deleteSubtree(node->left);
        deleteSubtree(node->right);
        delete node;
    }

    Node *copySubtree(const Node *node)
    {
        if (node == nullptr)
        {
            return nullptr;
        }

        return new Node(node->data, copySubtree(node->left), copySubtree(node->right));
    }

    [[nodiscard]] Node *rotateRight(Node *node)
    {
        Node *newRoot = node->left;
        Node *newRight = newRoot->right;
        newRoot->right = node;
        node->left = newRight;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        newRoot->height = std::max(height(newRoot->left), height(newRoot->right)) + 1;
        return newRoot;
    }

    [[nodiscard]] Node *rotateLeft(Node *node)
    {
        Node *newRoot = node->right;
        Node *newLeft = newRoot->left;
        newRoot->left = node;
        node->right = newLeft;
        node->height = std::max(height(node->left), height(node->right)) + 1;
        newRoot->height = std::max(height(newRoot->left), height(newRoot->right)) + 1;
        return newRoot;
    }

    inline HeightType height(Node *node) const noexcept
    {
        return node ? node->height : 0;
    }

    inline HeightType getBalance(Node *node) const noexcept
    {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void printSubtree(const Node *node, int indent) const
    {
        if (node == nullptr)
        {
            std::cout << std::setw(indent) << ' ';
            std::cout << '.' << '\n';
            return;
        }

        // Print right subtree recursively with increased indentation
        printSubtree(node->right, indent + 4);

        // Print current node
        std::cout << std::setw(indent) << ' ';
        std::cout << node->data << '\n';

        // Print left subtree recursively with increased indentation
        printSubtree(node->left, indent + 4);
    }

  private:
    Node *m_root = nullptr;
};

} // namespace icb
