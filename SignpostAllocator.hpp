#pragma once

struct Signpost {
	Signpost* m_next;
	Signpost* m_prev;
	size_t m_size;
	bool m_free;
};

class SignpostAllocator
{
public:
	SignpostAllocator();
	~SignpostAllocator();

	void Init(size_t initial_size);
	void VerifyLinkedList();
	void* Alloc(size_t size);

	void Deinit();
	void Free(void* ptr);
public:
	void* m_buffer;
	Signpost* m_posts;
};

