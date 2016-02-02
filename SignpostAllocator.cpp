#include "SignpostAllocator.hpp"
#include <crtdbg.h>
#include "Assert.hpp"

void SignpostAllocator::Init(size_t initial_size) {
	m_buffer = malloc(initial_size);

	Signpost* post = (Signpost*)m_buffer;
	post->m_size = initial_size;
	post->m_next = nullptr;
	post->m_prev = nullptr;

	post->m_free = true;
	m_posts = post;
}


SignpostAllocator::SignpostAllocator()
{

}

void SignpostAllocator::VerifyLinkedList() {
	Signpost* iter = m_posts;
	while (iter != nullptr) {
		bool verifyNext = true;
		bool verifyPrev = true;

		if (iter->m_next)
			verifyPrev = iter->m_next->m_prev == iter;

		if (iter->m_prev)
			verifyNext = iter->m_prev->m_next == iter;

		FATAL_ASSERT(verifyPrev);
		FATAL_ASSERT(verifyNext);
		iter = iter->m_next;
	}
}

void* SignpostAllocator::Alloc(size_t size) {
	VerifyLinkedList();
	//size_t alloc_size = sizeof(Signpost);
	Signpost* iter = m_posts;

	while (iter != nullptr) {
		if (iter->m_free && iter->m_size >= size + sizeof(Signpost))
			break;
		iter = iter->m_next;
	}

	if (iter == nullptr)
		return nullptr;

	iter->m_free = false;

	char* iter_buffer = (char*)iter + sizeof(Signpost);
	Signpost* next_signpost = (Signpost*)(iter_buffer + size);
	next_signpost->m_free = true;
	next_signpost->m_prev = iter;
	next_signpost->m_next = iter->m_next;
	next_signpost->m_size = iter->m_size - sizeof(Signpost) - size;

	iter->m_size = size;

	if (iter->m_next)
		iter->m_next->m_prev = next_signpost;

	iter->m_next = next_signpost;

	VerifyLinkedList();
	return iter_buffer;
}

void SignpostAllocator::Deinit() {
	FATAL_ASSERT(m_posts->m_next == nullptr && m_posts->m_prev == nullptr && m_posts->m_free);
	free(m_buffer);
}

void SignpostAllocator::Free(void* ptr) {
	char* iter_buffer = (char*)ptr;
	Signpost* post = (Signpost*)(iter_buffer - sizeof(Signpost));
	FATAL_ASSERT(post->m_free == false);

	//check prev or next, see if they're free, or both are free, and combine
	if ((post->m_prev != nullptr) && (post->m_prev->m_free) && (post->m_next != nullptr) && (post->m_next->m_free)) {
		size_t size = post->m_prev->m_size + post->m_size + post->m_next->m_size + 2 * sizeof(Signpost);

		Signpost* prev = post->m_prev;

		prev->m_size = size;

		prev->m_next = post->m_next->m_next;

		if (post->m_next->m_next)
			post->m_next->m_next->m_prev = prev; //////
	}
	else if ((post->m_prev != nullptr) && (post->m_prev->m_free)) {
		size_t size = post->m_prev->m_size + post->m_size + sizeof(Signpost);

		Signpost* prev = post->m_prev;
		prev->m_size = size;

		prev->m_next = post->m_next;

		post->m_next->m_prev = prev; /////
	}
	else if ((post->m_next != nullptr) && (post->m_next->m_free)) {
		post->m_size = post->m_next->m_size + post->m_size + sizeof(Signpost);

		bool hasNextNext = post->m_next->m_next != nullptr;
		if (hasNextNext) {
			post->m_next->m_next->m_prev = post; //
		}

		post->m_next = post->m_next->m_next;

		post->m_free = true;
	}
	else {
		post->m_free = true;
	}

	VerifyLinkedList();
}