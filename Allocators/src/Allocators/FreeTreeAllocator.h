#pragma once

#include "Allocator.h"

#include "Containers/RedBlackTree.h"

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
        Init();
    }

    void free(void* ptr) override;

protected:
    void Init() override;
    void* AllocImpl(size_t size) override;

private:
    Node* Find(size_t size)
    {
        constexpr size_t HeaderSize = sizeof(AllocHeader);

        return mFreeTree.best(size + HeaderSize);
    }

    void Coalesce(Node* freeNode);

private:
    void* mMemBlock = nullptr;
    HeaderTree mFreeTree;
};