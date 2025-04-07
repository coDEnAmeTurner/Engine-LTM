#include "LF_LinkedList.h"	
#include <thread>
#include <iostream>

template<class T>
void LF_LinkedList<T>::push_front(T data) {
	auto pNode = new Node();
	pNode->m_data = data;
	pNode->m_next = m_head;

	while (!m_head.compare_exchange_weak(pNode->m_next, pNode)) 
	{
		_mm_pause();
	}
}

template<class T>
T LF_LinkedList<T>::front() {
	auto pNode = m_head.load(std::memory_order_acquire);

	if (!pNode) return -1;

	return pNode->m_data;
}

template<class T>
void LF_LinkedList<T>::pop() {
	if (m_head.load(std::memory_order_acquire) == nullptr) {
		return; // List is empty
	}
	auto pNode = m_head.load(std::memory_order_acquire);
	if (!pNode) { //invalid
		return;
	}
	m_head.store(m_head.load(std::memory_order_relaxed)->m_next, std::memory_order_release);
	delete pNode;
}

LF_LinkedList<int> list;

void produce_into_LF_linked_list() {
	for (int i = 0; i < 10; ++i) {
		list.push_front(i);
	}
}
void consume_from_LF_linked_list() {
	for (int i = 0; i < 10; ++i) {
		std::cout << list.front() << std::endl; // Should print 9, 8, ..., 0
		list.pop();
	}
}