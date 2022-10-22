#pragma once

#include "Allocator.h"

class FreeListAllocator : public Allocator
{
public:
    struct Node
    {
        size_t blockSize = 0;
        Node* next = nullptr;

        Node() = default;
        Node(size_t data) : blockSize(data) {}
    };

    struct AllocHeader
    {
        size_t blockSize = 0;
    };

public:
    FreeListAllocator(size_t maxSize)
        : Allocator(maxSize)
    {
        Init();
    }
    virtual ~FreeListAllocator()
    {
        ::operator delete(mMemBlock);
        mMemBlock = nullptr;
        mHead = nullptr;
        mUsed = 0;
    }

    void free(void* ptr) override
    {
        // TODO
    }

protected:
    void Init() override
    {
        if (mMemBlock)
        {
            ::operator delete(mMemBlock);
            mMemBlock = nullptr;
            mHead = nullptr;
            mUsed = 0;
        }

        mMemBlock = ::operator new(mMaxSize);
        mHead = new(mMemBlock) Node(mMaxSize);
    }

    void* AllocImpl(size_t size) override
    {
        constexpr size_t NodeSize = sizeof(Node);
        constexpr size_t HeaderSize = sizeof(AllocHeader);

        Node* bestNode = Find(size);
        ASSERT(bestNode, "No free memory available");

        size_t requiredSize = size + HeaderSize;

        // TODO
        return nullptr;
    }

private:
    Node* Find(size_t size)
    {
        Node* node = mHead;
        while (node)
        {
            if (size < node->blockSize)
                return node;

            node = node->next;
        }

        return nullptr;
    }

private:
    void* mMemBlock = nullptr;
    Node* mHead = nullptr;
};