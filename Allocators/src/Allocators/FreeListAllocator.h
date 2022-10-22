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
    void Init() override;
    void* AllocImpl(size_t size) override;

private:
    Node* Find(size_t size);

private:
    void* mMemBlock = nullptr;
    Node* mHead = nullptr;
};