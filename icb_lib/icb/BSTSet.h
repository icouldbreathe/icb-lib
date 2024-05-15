#pragma once

#include <iostream>
#include <functional>

namespace icb
{
    template<typename T>
    class BSTSet
    {
        public:
            using SizeType = size_t;
            using ValueType = T;

        private:
            struct Node
            {
                ValueType data;
                Node *left;
                Node *right;

                Node(const ValueType &data)
                    : data(data), left(nullptr), right(nullptr)
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
            BSTSet() = default;
            ~BSTSet()
            {
                deleteSubtree(m_root);
            }

            // copy ctor
            BSTSet(const BSTSet<T> &other) noexcept
            {
                if (other.m_root == nullptr)
                {
                    return;
                }

                m_root = copySubtree(other.m_root);
            }

            // move ctor
            BSTSet(BSTSet<T> &&other) noexcept
                : m_root(other.m_root)
            {
                other.m_root = nullptr;
            }

            // copy assignment
            BSTSet operator=(const BSTSet<T> &other) noexcept
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
            BSTSet operator=(BSTSet &&other) noexcept
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
            //copy
            void Insert(const ValueType &data)
            {
                m_root = insertAt(m_root, data);
            }
            // move
            void Insert(ValueType &&data)
            {
                m_root = insertAt(m_root, std::move(data));
            }

            bool Contains(const ValueType &data) const noexcept
            {
                return containsAt(m_root, data);
            }

            void ForEachNode(TraversalOrder order, std::function<void(const ValueType&)> fn) const noexcept
            {
                switch(order)
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
            
        private:
            void inorderTraversal(Node *node, std::function<void(const ValueType&)> fn) const noexcept
            {
                if (node == nullptr)
                {
                    return;
                }

                inorderTraversal(node->left, fn);

                fn(node->data);

                inorderTraversal(node->right, fn);

            }

            void preorderTraversal(Node *node, std::function<void(const ValueType&)> fn) const noexcept
            {
                if (node == nullptr)
                {
                    return;
                }

                fn(node->data);

                preorderTraversal(node->left, fn);

                preorderTraversal(node->right, fn);
            }

            void postorderTraversal(Node *node, std::function<void(const ValueType&)> fn) const noexcept
            {
                if (node == nullptr)
                {
                    return;
                }

                postorderTraversal(node->left, fn);

                postorderTraversal(node->right, fn);

                fn(node->data);
            }

            // copy
            Node *insertAt(Node *node, const ValueType &data)
            {
                if (node == nullptr)
                {
                    return new Node(data);
                }

                if (data < node->data)
                {
                    node->left = insertAt(node->left, data);
                }
                else if (data > node->data)
                {
                    node->right = insertAt(node->right, data);
                }

                return node;
            }

            // move
            Node *insertAt(Node *node, ValueType &&data)
            {
                if (node == nullptr)
                {
                    return new Node(std::move(data));
                }

                if (data < node->data)
                {
                    node->left = insertAt(node->left, std::move(data));
                }
                else if (data > node->data)
                {
                    node->right = insertAt(node->right, std::move(data));
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

        private:
            Node *m_root = nullptr;
    };

} // namespace icb