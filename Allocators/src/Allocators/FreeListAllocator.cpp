#include "FreeListAllocator.h"

void FreeListAllocator::Init()
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

void* FreeListAllocator::AllocImpl(size_t size)
{
    constexpr size_t NodeSize = sizeof(Node);
    constexpr size_t HeaderSize = sizeof(AllocHeader);

    Node* bestNode = Find(size);
    ASSERT(bestNode, "No free memory available");

    size_t requiredSize = size + HeaderSize;

    // TODO
    return nullptr;
}

FreeListAllocator::Node* FreeListAllocator::Find(size_t size)
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
