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
void RunBenchmark()
{
	Benchmark<T> benchmarker;
	benchmarker.run();
}

int main()
{
	RunBenchmark<DefaultAllocator>();
	RunBenchmark<LinearAllocator>();
	RunBenchmark<StackAllocator>();
	RunBenchmark<PoolAllocator>();
	RunBenchmark<FreeTreeAllocator>();
}