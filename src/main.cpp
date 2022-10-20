#include <iostream>

#include "FreeTreeAllocator.h"

template<size_t size>
struct TestStruct
{
	uint8_t data[size];
};

using TestStruct64 = TestStruct<64>;
using TestStruct256 = TestStruct<256>;

int main()
{
	FreeTreeAllocator* allocator = new FreeTreeAllocator(4096);

	TestStruct64* test = allocator->alloc<TestStruct64>();

	for (size_t i = 0; i < 64; i++)
	{
		test->data[i] = (uint8_t)i;
	}

	TestStruct64* test2 = allocator->alloc<TestStruct64>();
	allocator->free(test);

	TestStruct256* test3 = allocator->alloc<TestStruct256>();
}