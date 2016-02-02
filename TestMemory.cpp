#include "TestMemory.hpp"
#include "Utilities\EngineCommon.hpp"
#include "SignpostAllocator.hpp"
#include <algorithm>
#include <xutility>


TestMemory::TestMemory() :
m_signpostAllocator(new SignpostAllocator())
{
	m_signpostAllocator->Init(10000000);
}


TestMemory::~TestMemory()
{
}

void TestMemory::TestAllocator() {

	for (int x = 0; x < 1000; x++) {
		void* thing = m_signpostAllocator->Alloc(RandomInt(10, 100));
		m_ptrPool.push_back(thing);

		if (RandomInt(1, 1000) % 5 == 0) {
			void* randomElement = RandomElement();
			m_signpostAllocator->Free(randomElement);
			m_ptrPool.erase(std::remove(m_ptrPool.begin(), m_ptrPool.end(), randomElement), m_ptrPool.end());
		}

}

	for (int x = 0; x < 100; x++) {
		void* randomElement = RandomElement();
		m_signpostAllocator->Free(randomElement);
		m_ptrPool.erase(std::remove(m_ptrPool.begin(), m_ptrPool.end(), randomElement), m_ptrPool.end());
	}

	for (int x = 0; x < 1000; x++) {
		void* thing = m_signpostAllocator->Alloc(RandomInt(50, 500));
		m_ptrPool.push_back(thing);
	}

	for (int x = 0; x < 100; x++) {
		void* randomElement = RandomElement();
		m_signpostAllocator->Free(randomElement);
		m_ptrPool.erase(std::remove(m_ptrPool.begin(), m_ptrPool.end(), randomElement), m_ptrPool.end());
	}

	for (int x = 0; x < 1000; x++) {
		void* randomElement = RandomElement();
		m_signpostAllocator->Free(randomElement);
		m_ptrPool.erase(std::remove(m_ptrPool.begin(), m_ptrPool.end(), randomElement), m_ptrPool.end());
	}

}

void* TestMemory::RandomElement() {
	auto it = m_ptrPool.begin();
	const unsigned long n = std::distance(it, m_ptrPool.end());
	const unsigned long divisor = (RAND_MAX + 1) / n;

	unsigned long k;
	do { k = std::rand() / divisor; } while (k >= n);

	std::advance(it, k);
	return *it;
}
