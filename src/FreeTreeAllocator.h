#pragma once

#include "Allocator.h"
#include "RedBlackTree.h"

class FreeTreeAllocator : public Allocator
{
public:
    struct AllocHeader
    {
        size_t prevBlockSize = 0;
        size_t blockSize = 0;
        bool free = true;

        bool operator==(const AllocHeader& other) const { return blockSize == other.blockSize; }
        bool operator<(const AllocHeader& other) const { return blockSize < other.blockSize; }
        bool operator<=(const AllocHeader& other) const { return blockSize <= other.blockSize; }
        bool operator>(const AllocHeader& other) const { return blockSize > other.blockSize; }

        AllocHeader() = default;
        AllocHeader(size_t size) : blockSize(size) {}
    };

    using HeaderTree = RedBlackTree<AllocHeader>;
    using Node = HeaderTree::Node;

public:
    FreeTreeAllocator(size_t maxSize)
        : Allocator(maxSize) 
    {
        init();
    }

    void init() override
    {
        if (mMemBlock)
        {
            ::operator delete(mMemBlock);
             mMemBlock = nullptr;
        }

        mFreeTree.clear();
        mMemBlock = ::operator new(mMaxSize);

        Node* firstNode = new(mMemBlock) Node();
        firstNode->data.prevBlockSize = 0;
        firstNode->data.blockSize = mMaxSize;
        mFreeTree.insert(firstNode);
    }

    void free(void* ptr) override
    {
        constexpr size_t HeaderSize = sizeof(AllocHeader);

        size_t headerAddress = (size_t)ptr - HeaderSize;
        size_t blockSize = ((AllocHeader*)headerAddress)->blockSize;

        Node* freeNode = new((void*)headerAddress) Node();
        freeNode->data.blockSize = blockSize;
        freeNode->colour = HeaderTree::RED;
        mFreeTree.insert(freeNode);

        freeNode->data.free = true;
        mUsed -= freeNode->data.blockSize;
        Coalesce(freeNode);
    }

protected:
    void* AllocImpl(size_t size) override
    {
        constexpr size_t NodeSize = sizeof(Node);
        constexpr size_t HeaderSize = sizeof(AllocHeader);

        Node* bestNode = Find(size);
        ASSERT(bestNode, "No free memory available!");

        size_t requiredSize = size + HeaderSize;
        if (bestNode->data.blockSize - requiredSize > NodeSize)
        {
            // If there is space for a free node remaining then add one or expand the following node if it is also free.
            size_t nextBlock = (size_t)bestNode + bestNode->data.blockSize;
            AllocHeader* nextData = nextBlock < (size_t)mMemBlock + mMaxSize ? (AllocHeader*)nextBlock : nullptr;
            if (nextData && nextData->free)
            {
                // Next block is free, so expand with unused memory from requested memory.
                Node* nextNode = (Node*)nextData;
                Node* newNextNode = new(bestNode + requiredSize) Node();
                *newNextNode = *nextNode;
                nextNode->~Node();

                newNextNode->data.blockSize += bestNode->data.blockSize - requiredSize;
                newNextNode->data.prevBlockSize = requiredSize;
            }
            else
            {
                // Next block is not free, add a new free block to the tree for unused memory block.
                size_t newNextSize = bestNode->data.blockSize - requiredSize;
                Node* newNextNode = new((void*)((size_t)bestNode + requiredSize)) Node();
                newNextNode->data.blockSize = newNextSize;
                newNextNode->data.prevBlockSize = requiredSize;
                newNextNode->colour = HeaderTree::RED;
                mFreeTree.insert(newNextNode);
            }
        }
        else
            requiredSize = bestNode->data.blockSize;

        mFreeTree.remove(bestNode);

        size_t returnAddress = (size_t)bestNode + HeaderSize;
        mUsed += requiredSize; 

        AllocHeader* header = (AllocHeader*)bestNode;
        header->blockSize = requiredSize;
        header->free = false;

        return (void*)returnAddress;
    }

private:
    Node* Find(size_t size)
    {
        constexpr size_t HeaderSize = sizeof(AllocHeader);

        return mFreeTree.best(size + HeaderSize);
    }

    void Coalesce(Node* freeNode)
    {
        if ((size_t)freeNode + freeNode->data.blockSize < (size_t)mMemBlock + mMaxSize)
        {
            AllocHeader* nextData = (AllocHeader*)((size_t)freeNode + freeNode->data.blockSize);
            if (nextData->free)
            {
                Node* nextNode = (Node*)nextData;
                freeNode->data.blockSize += nextNode->data.blockSize;

                Node* newNextNode = (Node*)((size_t)nextNode + nextNode->data.blockSize);
                newNextNode->data.prevBlockSize = freeNode->data.blockSize;

                mFreeTree.remove(nextNode);
            }
        }

        if (freeNode->data.prevBlockSize <= 0)
            return;

        if ((size_t)freeNode - freeNode->data.prevBlockSize > (size_t)mMemBlock)
        {
            AllocHeader* prevData = (AllocHeader*)((size_t)freeNode - freeNode->data.prevBlockSize);
            if (prevData->free)
            {
                Node* prevNode = (Node*)prevData;
                prevNode->data.blockSize += freeNode->data.blockSize;

                Node* newNextNode = (Node*)((size_t)freeNode + freeNode->data.blockSize);
                newNextNode->data.prevBlockSize = prevNode->data.blockSize;

                mFreeTree.remove(freeNode);
            }
        }
    }

private:
    void* mMemBlock = nullptr;
    HeaderTree mFreeTree;
};