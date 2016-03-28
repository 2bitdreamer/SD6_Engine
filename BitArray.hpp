#include "Assert.hpp"
typedef unsigned int uint_t;

class BitArray
{
public:
	BitArray(void *buffer, size_t byte_len, bool initial_value);

	void reset(bool value);

	void set(uint_t idx);

	void unset(uint_t idx);

	bool get(uint_t idx) const;

private:
	unsigned char *m_buffer;
	size_t m_buffer_len;
};

template <size_t BIT_SIZE>
class TBitArray : public BitArray
{
public:
	TBitArray(bool initial_value)
		: BitArray(m_fixed_buffer, (BIT_SIZE + 7) / 8, initial_value)
	{}

	TBitArray()
		: BitArray(m_fixed_buffer, (BIT_SIZE + 7) / 8, false)
	{}

private:
	unsigned char m_fixed_buffer[(BIT_SIZE + 7) / 8];
};

