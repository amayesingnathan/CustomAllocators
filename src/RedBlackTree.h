#pragma once

#include "Common.h"

template<typename T>
class RedBlackTree
{
public:
    enum Colour : uint64_t
    {
        RED, BLACK
    };

    struct Node
    {
        T data;

        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;
        Colour colour = RED;

        Node() : data() {}
        Node(const T& _data) : data(_data) {}

    private:
        bool owned = false;
        Node(const T& _data, bool _owned) : data(_data), owned(_owned) {}

        template<typename T>
        friend class RedBlackTree;
    };

private:
    using NodePtr = Node*;

    NodePtr mNull = nullptr;
    NodePtr mRoot = nullptr;

public:
    RedBlackTree()
    {
        mNull = new Node;
        mNull->colour = BLACK;
        mRoot = mNull;
    }
    ~RedBlackTree() { delete mNull; }

    void insert(const T& key)
    {
        NodePtr node = new Node(key, true);
        insert(node);
    }

    void insert(NodePtr newNode)
    {
        newNode->left = mNull;
        newNode->right = mNull;

        NodePtr y = nullptr;
        NodePtr x = mRoot;

        while (x != mNull)
        {
            y = x;
            if (newNode->data < x->data)
                x = x->left;
            else
                x = x->right;
        }

        newNode->parent = y;
        if (!y)
            mRoot = newNode;
        else if (newNode->data < y->data)
            y->left = newNode;
        else
            y->right = newNode;

        if (!newNode->parent)
        {
            newNode->colour = BLACK;
            return;
        }

        if (!newNode->parent->parent)
            return;

        PostInsertFix(newNode);
    }

    void remove(NodePtr node) { DeleteImpl(mRoot, node); }

    NodePtr find(const T& k) const { return FindR(mRoot, k); }

    NodePtr min(NodePtr node = mRoot) const
    {
        ASSERT(node);
        while (node->left != mNull) {
            node = node->left;
        }
        return node;
    }
    NodePtr max(NodePtr node = mRoot) const
    {
        ASSERT(node);
        while (node->right != mNull) {
            node = node->right;
        }
        return node;
    }

    NodePtr next(NodePtr node) const
    {
        ASSERT(node);

        if (node->right != mNull)
            return min(node->right);

        NodePtr y = node->parent;
        while (y != mNull && node == y->right)
        {
            node = y;
            y = y->parent;
        }
        return y;
    }

    NodePtr prev(NodePtr node) const
    {
        ASSERT(node);

        if (node->left != mNull)
            return max(node->left);

        NodePtr y = node->parent;
        while (y != mNull && node == y->left)
        {
            node = y;
            y = y->parent;
        }
        return y;
    }

    NodePtr root() const { return mRoot; }

    void clear() { DeleteR(mRoot); }

    NodePtr best(const T& val) { return BestFitImpl(mRoot, val); }

private:
    void PostInsertFix(NodePtr node)
    {
        NodePtr hold;
        while (node->parent->colour == RED)
        {
            if (node->parent == node->parent->parent->right)
            {
                hold = node->parent->parent->left;
                if (hold->colour == RED)
                {
                    hold->colour = BLACK;
                    node->parent->colour = BLACK;
                    node->parent->parent->colour = RED;
                    node = node->parent->parent;
                }
                else
                {
                    if (node == node->parent->left)
                    {
                        node = node->parent;
                        RightRotate(node);
                    }
                    node->parent->colour = BLACK;
                    node->parent->parent->colour = RED;
                    LeftRotate(node->parent->parent);
                }
            }
            else
            {
                hold = node->parent->parent->right;

                if (hold->colour == RED)
                {
                    hold->colour = BLACK;
                    node->parent->colour = BLACK;
                    node->parent->parent->colour = RED;
                    node = node->parent->parent;
                }
                else
                {
                    if (node == node->parent->right)
                    {
                        node = node->parent;
                        LeftRotate(node);
                    }
                    node->parent->colour = BLACK;
                    node->parent->parent->colour = RED;
                    RightRotate(node->parent->parent);
                }
            }
            if (node == mRoot)
                break;
        }
        mRoot->colour = BLACK;
    }

    void PostDeleteFix(NodePtr x)
    {
        NodePtr s;
        while (x != mRoot && x->colour == BLACK)
        {
            if (x == x->parent->left)
            {
                s = x->parent->right;
                if (s->colour == RED)
                {
                    s->colour = BLACK;
                    x->parent->colour = RED;
                    LeftRotate(x->parent);
                    s = x->parent->right;
                }

                if (s->left->colour == BLACK && s->right->colour == BLACK)
                {
                    s->colour = RED;
                    x = x->parent;
                }
                else
                {
                    if (s->right->colour == BLACK)
                    {
                        s->left->colour = BLACK;
                        s->colour = RED;
                        RightRotate(s);
                        s = x->parent->right;
                    }

                    s->colour = x->parent->colour;
                    x->parent->colour = BLACK;
                    s->right->colour = BLACK;
                    LeftRotate(x->parent);
                    x = mRoot;
                }
            }
            else
            {
                s = x->parent->left;
                if (s->colour == RED)
                {
                    s->colour = BLACK;
                    x->parent->colour = RED;
                    RightRotate(x->parent);
                    s = x->parent->left;
                }

                if (s->left->colour == BLACK && s->right->colour == BLACK) // Maybe just s->right???
                {
                    s->colour = RED;
                    x = x->parent;
                }
                else
                {
                    if (s->left->colour == BLACK)
                    {
                        s->right->colour = BLACK;
                        s->colour = RED;
                        LeftRotate(s);
                        s = x->parent->left;
                    }

                    s->colour = x->parent->colour;
                    x->parent->colour = BLACK;
                    s->left->colour = BLACK;
                    RightRotate(x->parent);
                    x = mRoot;
                }
            }
        }

        x->colour = BLACK;
    }

    void FindR(NodePtr node, const T& key) const
    {
        if (node == mNull || key == node->data)
            return node;

        if (key < node->data)
            return FindR(node->left, key);

        return FindR(node->right, key);
    }

    void DeleteR(NodePtr node)
    {
        if (!node)
            return;

        DeleteR(node->left);
        DeleteR(node->right);

        if (node->owned)
            delete node;
        else
            memset(node, 0, sizeof(Node));

        node = nullptr;
    }

    void DeleteImpl(NodePtr start, NodePtr node)
    {
        NodePtr z = mNull;
        NodePtr x, y;

        while (start != mNull)
        {
            if (start->data == node->data)
                z = start;

            if (start->data <= node->data)
                start = start->right;
            else
                start = start->left;
        }

        if (z == mNull)
            return;

        y = z;
        Colour yOrigCol = y->colour;

        if (z->left == mNull)
        {
            x = z->right;
            Transplant(z, z->right);
        }
        else if (z->right == mNull)
        {
            x = z->left;
            Transplant(z, z->left);
        }
        else
        {
            y = min(z->right);
            yOrigCol = y->colour;
            x = y->right;
            if (y->parent == z)
                x->parent = y;
            else
            {
                Transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }

            Transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->colour = z->colour;
        }

        if (z->owned)
            delete z;
        else
            memset(node, 0, sizeof(Node));

        if (yOrigCol == BLACK)
            PostDeleteFix(x);
    }

    void Transplant(NodePtr u, NodePtr v)
    {
        if (!u->parent)
            mRoot = v;
        else if (u == u->parent->left)
            u->parent->left = v;
        else
            u->parent->right = v;

        v->parent = u->parent;
    }

    void LeftRotate(NodePtr x)
    {
        NodePtr y = x->right;
        x->right = y->left;
        if (y->left != mNull)
            y->left->parent = x;

        y->parent = x->parent;
        if (!x->parent)
            mRoot = y;
        else if (x == x->parent->left)
            x->parent->left = y;
        else
            x->parent->right = y;

        y->left = x;
        x->parent = y;
    }

    void RightRotate(NodePtr x)
    {
        NodePtr y = x->left;
        x->left = y->right;
        if (y->left != mNull)
            y->left->parent = x;

        y->parent = x->parent;
        if (!x->parent)
            mRoot = y;
        else if (x == x->parent->right)
            x->parent->right = y;
        else
            x->parent->left = y;

        y->right = x;
        x->parent = y;
    }

    NodePtr BestFitImpl(NodePtr node, const T& val)
    {
        if (!node)
            return nullptr;

        if (node->data == val)
            return node;

        if (node->data < val)
            return BestFitImpl(node->right, val);

        if (node->data > val)
        {
            NodePtr result = BestFitImpl(node->left, val);
            if (!result)
                return node;
            return result;
        }

        return nullptr;
    }
};