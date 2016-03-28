#include "BitArray.hpp"

BitArray::BitArray(void *buffer, size_t byte_len, bool initial_value)
{
	m_buffer = (unsigned char*)buffer;
	m_buffer_len = byte_len;
	reset(initial_value);
}

void BitArray::reset(bool value)
{
	if (value) {
		memset(m_buffer, 0xff, m_buffer_len);
	}
	else {
		memset(m_buffer, 0x00, m_buffer_len);
	}
}

void BitArray::set(uint_t idx)
{
	uint_t const byte_index = idx / 8;
	uint_t const bit_index = idx % 8;
	FATAL_ASSERT(byte_index < m_buffer_len);

	unsigned char b = m_buffer[byte_index];
	b = b | (1 << bit_index);
	m_buffer[byte_index] = b;
}

void BitArray::unset(uint_t idx)
{
	uint_t const byte_index = idx / 8;
	uint_t const bit_index = idx % 8;
	FATAL_ASSERT(byte_index < m_buffer_len);

	unsigned char b = m_buffer[byte_index];
	b = b & ~(1 << bit_index);
	m_buffer[byte_index] = b;
}

bool BitArray::get(uint_t idx) const
{
	uint_t const byte_index = idx / 8;
	uint_t const bit_index = idx % 8;
	FATAL_ASSERT(byte_index < m_buffer_len);

	unsigned char const b = m_buffer[byte_index];
	return (b & (1 << bit_index)) != 0;
}

