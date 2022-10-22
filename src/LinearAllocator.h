#pragma once

#include "Allocator.h"

class LinearAllocator : public Allocator
{
public:
    LinearAllocator(size_t maxSize)
        : Allocator(maxSize)
    {
        Init();
    }

    virtual ~LinearAllocator()
    {
        ::operator delete(mMemBlock);
        mMemBlock = nullptr;
        mUsed = 0;
    }

    void free(void* ptr) override
    {
        // Does nothing in a linear allocator, must reset to reuse block
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
        ASSERT(mUsed + size <= mMaxSize, "No free memory!");

        size_t newAddress = (size_t)mMemBlock + mUsed;
        mUsed += size;
        return (void*)newAddress;
    }

private:
    void* mMemBlock = nullptr;
};