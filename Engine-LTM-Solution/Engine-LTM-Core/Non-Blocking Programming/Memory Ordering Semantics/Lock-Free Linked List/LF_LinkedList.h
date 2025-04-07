#pragma once
#include <atomic>

template<class T>

class LF_LinkedList {
	struct Node {
		T m_data;
		Node* m_next;
	};

	std::atomic<Node*> m_head{ nullptr };
public:
	void push_front(T data);
	T front();
	void pop();
};