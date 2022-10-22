#include "LinearAllocator.h"

void LinearAllocator::Init()
{
    if (mMemBlock)
    {
        ::operator delete(mMemBlock);
        mMemBlock = nullptr;
        mUsed = 0;
    }

    mMemBlock = ::operator new(mMaxSize);
}

void* LinearAllocator::AllocImpl(size_t size)
{
    ASSERT(mUsed + size <= mMaxSize, "No free memory!");

    size_t newAddress = (size_t)mMemBlock + mUsed;
    mUsed += size;
    return (void*)newAddress;
}
