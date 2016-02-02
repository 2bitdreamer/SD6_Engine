#pragma once
#include <vector>
class SignpostAllocator;

class TestMemory
{
public:
	TestMemory();
	~TestMemory();

	void TestAllocator();
	void* RandomElement();
public:
	SignpostAllocator* m_signpostAllocator;
	std::vector<void*> m_ptrPool;
};

