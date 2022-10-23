#include "Allocators.h"

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