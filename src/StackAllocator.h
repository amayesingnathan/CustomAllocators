#pragma once

#include "Allocator.h"

class StackAllocator : public Allocator
{
public:
    struct AllocHeader
    {
        size_t blockSize;
    };

public:
    StackAllocator(size_t maxSize)
        : Allocator(maxSize)
    {
        Init();
    }

    virtual ~StackAllocator()
    {
        ::operator delete(mMemBlock);
        mMemBlock = nullptr;
        mUsed = 0;
    }

    void free(void* ptr) override
    {
        constexpr size_t HeaderSize = sizeof(AllocHeader);
        size_t ptrAddress = (size_t)ptr;
        size_t headerAddress = ptrAddress - HeaderSize;
        void* headerPtr = (void*)headerAddress;
        AllocHeader* header = (AllocHeader*)headerAddress;

        size_t lastAddress = (size_t)mMemBlock + mUsed - header->blockSize;
        ASSERT(ptrAddress == lastAddress, "Can only free the last allocated block!");

        mUsed -= header->blockSize + HeaderSize;
    }

    void reset() { Init(); }

protected:  
    void Init() override
    {
        if (mMemBlock)
        {
            ::operator delete(mMemBlock);
            mMemBlock = nullptr;
            mUsed = 0;
        }

        mMemBlock = ::operator new(mMaxSize);
    }

    void* AllocImpl(size_t size) override
    {
        constexpr size_t HeaderSize = sizeof(AllocHeader);
        ASSERT(mUsed + size + HeaderSize < mMaxSize, "No free memory!");

        ((AllocHeader*)((size_t)mMemBlock + mUsed))->blockSize = size;
        size_t newAddress = (size_t)mMemBlock + mUsed + HeaderSize;
        mUsed += size + HeaderSize;
        return (void*)newAddress;
    }

private:
    void* mMemBlock = nullptr;
};