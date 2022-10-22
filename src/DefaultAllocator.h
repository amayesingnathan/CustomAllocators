#pragma once

#include "Allocator.h"

class DefaultAllocator : public Allocator
{
public:
    DefaultAllocator() = default;

    void free(void* ptr) override
    {
        ::operator delete(ptr);
    }

protected:
    void Init() override {}

    void* AllocImpl(size_t size) override
    {
        return ::operator new(size);
    }
};