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

    void free(void* ptr) override;

    void reset() { Init(); }

protected:  
    void Init() override;
    void* AllocImpl(size_t size) override;

private:
    void* mMemBlock = nullptr;
};