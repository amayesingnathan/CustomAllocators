#include "PoolAllocator.h"

void PoolAllocator::free(void* ptr)
{
    if (!ptr)
        return;

    Node* block = (Node*)ptr;
    block->next = mHead;
    mHead = block;
}

void PoolAllocator::Init()
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

void* PoolAllocator::AllocImpl(size_t size)
{
    ASSERT(size == mBlockSize, "Allocation must be equal to block size!");

    Node* freeBlock = mHead;
    mHead = mHead->next;

    return (void*)freeBlock;
}
