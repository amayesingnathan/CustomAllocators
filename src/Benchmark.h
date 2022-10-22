#pragma once

#include <sstream>
#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>

#include "Allocator.h"

#define OneMB 1048576
#define OneKB 1024

#define BLOCK_AMOUNT 256 * OneMB
#define ALLOC_AMOUNT 64

template<size_t size>
struct TestData
{
	uint8_t data[size];
};

struct BenchmarkResults
{
	size_t operations;
	std::chrono::milliseconds duration, allocDuration, freeDuration;
	float opsPerSec, allocPerSec, freePerSec;
	float timePerOp, timePerAlloc, timePerFree;
};

class DefaultAllocator;
class StackAllocator;
class PoolAllocator;

template<typename AllocT>
class Benchmark
{
public:
	enum LogPolicy
	{
		CONSOLE, FILE
	};

public:
	Benchmark(LogPolicy policy = CONSOLE)
		: mPolicy(policy)
	{
		if constexpr (std::is_same<AllocT, DefaultAllocator>::value)
			mAllocator = Make<AllocT>();
		else if constexpr (std::is_same<AllocT, PoolAllocator>::value)
			mAllocator = Make<AllocT>(BLOCK_AMOUNT, ALLOC_AMOUNT);
		else
			mAllocator = Make<AllocT>(BLOCK_AMOUNT);
	}

	void run()
	{
		using TestObj = TestData<ALLOC_AMOUNT>;
		size_t AllocCount;
		if constexpr (std::is_same<AllocT, StackAllocator>::value)
			AllocCount = (BLOCK_AMOUNT) / (ALLOC_AMOUNT + sizeof(StackAllocator::AllocHeader));
		else if constexpr (std::is_same<AllocT, FreeTreeAllocator>::value)
			AllocCount = BLOCK_AMOUNT / (ALLOC_AMOUNT + sizeof(FreeTreeAllocator::AllocHeader));
		else
			AllocCount = BLOCK_AMOUNT / ALLOC_AMOUNT;

		std::vector<TestObj*> allocData(AllocCount);
		mResults = BenchmarkResults();

		Begin();

		for (size_t i = 0; i < AllocCount; i++)
			allocData[i] = mAllocator->alloc<TestObj>();
		EndAlloc(AllocCount);

		for (size_t i = 0; i < AllocCount; i++)
		{
			if constexpr (std::is_same<AllocT, LinearAllocator>::value)
				continue;
			else if constexpr (std::is_same<AllocT, StackAllocator>::value)
				mAllocator->free(allocData[AllocCount - i - 1]);
			else
				mAllocator->free(allocData[i]);
		}
		EndFree(AllocCount);

		End(AllocCount);
		PrintResults();
	}

private:
	void Begin() { mStart = std::chrono::high_resolution_clock::now();  }
	void End(size_t opCount) 
	{ 
		mEnd = std::chrono::high_resolution_clock::now();

		mResults.duration = std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mStart);
		mResults.operations = opCount;
		mResults.opsPerSec = (float)opCount / ((float)mResults.duration.count() / 1000);
		mResults.timePerOp = 1 / mResults.opsPerSec;
	}

	void EndAlloc(size_t allocCount)
	{
		mEnd = std::chrono::high_resolution_clock::now();

		mResults.allocDuration = std::chrono::duration_cast<std::chrono::milliseconds>(mEnd - mStart);
		mResults.allocPerSec = (float)allocCount / ((float)mResults.allocDuration.count() / 1000);
		mResults.timePerAlloc = 1 / mResults.allocPerSec;
	}

	void EndFree(size_t freeCount)
	{
		auto freeEnd = std::chrono::high_resolution_clock::now();

		mResults.freeDuration = std::chrono::duration_cast<std::chrono::milliseconds>(freeEnd - mEnd);
		mResults.freePerSec = (float)freeCount / ((float)mResults.freeDuration.count() / 1000);
		mResults.timePerFree = 1 / mResults.freePerSec;
	}

	void PrintResults()
	{
		mTrace << "Testing" << Utils::GetName<AllocT>() << ":\n\n";
		mTrace << "\tAllocations:\n";
		mTrace << "\t\tDuration: " << mResults.allocDuration.count() << "ms\n";
		mTrace << "\t\tPer Second: " << mResults.allocPerSec << " allocations\n";
		mTrace << "\t\tPer Allocation: " << mResults.timePerAlloc << "ms\n";
		mTrace << "\n";
		mTrace << "\tFrees:\n";
		mTrace << "\t\tDuration: " << mResults.freeDuration.count() << "ms\n";
		mTrace << "\t\tPer Second: " << mResults.freePerSec << " frees\n";
		mTrace << "\t\tPer Free: " << mResults.timePerFree << "ms\n";
		mTrace << "\n";
		mTrace << "\tCombined:\n";
		mTrace << "\t\tDuration: " << mResults.duration.count() << "ms\n";
		mTrace << "\t\tPer Second: " << mResults.opsPerSec << " operations\n";
		mTrace << "\t\tPer Op: " << mResults.timePerOp << "ms\n";
		mTrace << "\n";

		switch (mPolicy)
		{
		case CONSOLE: std::cout << mTrace.rdbuf(); break;
		case FILE: 
		{
			std::ofstream file("AllocationBenchmark.log", std::ios::binary);
			file << mTrace.rdbuf();
			break;
		}
		default: std::cout << mTrace.rdbuf(); break;
		}

		mTrace.clear();
	}

private:
	Instance<AllocT> mAllocator;
	BenchmarkResults mResults;

	std::chrono::time_point<std::chrono::high_resolution_clock> mStart, mEnd;

	LogPolicy mPolicy;
	std::stringstream mTrace;
};