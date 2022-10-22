#pragma once

#include "Core/Common.h"

class Allocator
{
public:
    Allocator() = default;
    Allocator(size_t maxSize)
        : mMaxSize(maxSize) {
    }

    virtual ~Allocator() {}

    template<typename T>
    T* alloc() { return (T*)AllocImpl(sizeof(T)); }

    virtual void free(void* ptr) = 0;

protected:
    virtual void Init() = 0;
    virtual void* AllocImpl(size_t size) = 0;

protected:
    const size_t mMaxSize = 0;
    size_t mUsed = 0;
};