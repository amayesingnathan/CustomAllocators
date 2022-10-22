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

    void free(void* ptr) override
    {
        Node* block = (Node*)ptr;
        block->next = mHead;
        mHead = block;
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

        for (size_t blockAddress = (size_t)mMemBlock; blockAddress < (size_t)mMemBlock + mMaxSize; blockAddress += mBlockSize)
        {
            Node* newNode = (Node*)blockAddress;
            newNode->next = mHead;
            mHead = newNode;
        }
    }

    void* AllocImpl(size_t size) override
    {
        ASSERT(size == mBlockSize, "Allocation must be equal to block size!");

        Node* freeBlock = mHead;
        mHead = mHead->next;

        return (void*)freeBlock;
    }

private:
    void* mMemBlock = nullptr;
    Node* mHead = nullptr;

    size_t mBlockSize = 0;
};