#pragma once

#include "Allocator.h"

class PoolAllocator : public Allocator
{
public:
    struct Node
    {
        Node* next;
    };

public:
    PoolAllocator(size_t maxSize, size_t blockSize)
        : Allocator(maxSize), mBlockSize(blockSize)
    {
        ASSERT(mBlockSize > sizeof(Node), "Block size must be larger than linked list node!");
        ASSERT(mMaxSize % mBlockSize == 0, "Must be an integer number of blocks!");
        Init();
    }
    virtual ~PoolAllocator()
    {
        ::operator delete(mMemBlock);
        mMemBlock = nullptr;
        mHead = nullptr;
        mUsed = 0;
    }

    void free(void* ptr) override;

protected:
    void Init() override;
    void* AllocImpl(size_t size) override;

private:
    void* mMemBlock = nullptr;
    Node* mHead = nullptr;

    size_t mBlockSize = 0;
};