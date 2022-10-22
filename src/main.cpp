#include <iostream>
#include <memory>
#include <utility>

#include "DefaultAllocator.h"
#include "LinearAllocator.h"
#include "StackAllocator.h"
#include "PoolAllocator.h"
#include "FreeListAllocator.h"
#include "FreeTreeAllocator.h"

#include "Benchmark.h"

template<typename T>
void RunAllocTest()
{
	Benchmark<T> benchmarker;
	benchmarker.run<TenKB>();
}

int main()
{
	//RunAllocTest<DefaultAllocator>();
	RunAllocTest<LinearAllocator>();
	//RunAllocTest<StackAllocator>();
	//RunAllocTest<PoolAllocator>();
	//RunAllocTest<FreeTreeAllocator>();
}