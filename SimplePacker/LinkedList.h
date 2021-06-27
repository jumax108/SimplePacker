#pragma once

#include <malloc.h>

template<typename T>
class CLinkedList {
public:

	struct Node {
		T value;
		Node* parent;
		Node* child;

		inline Node* operator=(Node* const node) {
			value = node->value;
			parent = node->parent;
			child = node->child;

			return this;
		}

		inline bool operator==(Node& node) {
			return value == node.value && parent == node.parent && child == node.child;
		}

	};

	class iterator {
		public:
			iterator() {
				node = nullptr;
			}

			iterator(Node* const node) {
				this->node = (Node*)node;
			}

			inline iterator* operator=(Node* const node) {
				this->node = (Node*)node;

				return this;
			}

			inline T& operator*() {
				return node->value;
			}

			inline iterator* operator++() {
				node = node->child;
				return this;
			}

			inline iterator* operator--() {
				node = node->parent;
				return this;
			}

			inline bool operator!=(Node* const node) {
				return !(*(this->node) == *node);
			}

			inline bool operator==(Node* const node) {
				return *(this->node) == *node;
			}

		private:
			Node* node;
			friend class CLinkedList;
	};

	CLinkedList();

	void push_front(
		T value // ������ ��
	);
	void push_back(
		T value // ������ ��
	);
	void pop_front();
	void pop_back();

	void insert(
		T const value, // ���� ������ ������
		Node* parent  // �� ����� �ڽ����� ����˴ϴ�.
	);

	iterator erase(
		iterator iter // ���� ��带 �����մϴ�.
	);

	void clear();

	inline Node* begin() {
		return head.child;
	}

	inline Node* end() {
		return &tail;
	}

	~CLinkedList();

protected:
	Node head;
	Node tail;

};


template<typename T>
CLinkedList<T>::CLinkedList() {

	head.parent = nullptr;
	head.child = &tail;

	tail.parent = &head;
	tail.child = nullptr;
}

template<typename T>
CLinkedList<T>::~CLinkedList() {
	clear();
}

template<typename T>
void CLinkedList<T>::insert(T value, CLinkedList<T>::Node* parent) {

	Node* node = (CLinkedList::Node*)malloc(sizeof(CLinkedList::Node));

	node->value = value;
	node->parent = parent;

	parent->child->parent = node;
	node->child = parent->child;
	parent->child = node;

}

template<typename T>
void CLinkedList<T>::push_front(T const value) {
	Node* node = (CLinkedList::Node*)malloc(sizeof(CLinkedList::Node));

	node->value = value;

	node->parent = &head;
	node->child = head.child;

	head.child->parent = node;
	head.child = node;
}

template<typename T>
void CLinkedList<T>::push_back(T const value) {
	Node* node = (CLinkedList::Node*)malloc(sizeof(CLinkedList::Node));

	if (node == nullptr) {
		return;
	}

	node->value = value;
	node->parent = tail.parent;
	node->child = &tail;

	tail.parent->child = node;
	tail.parent = node;
}


template<typename T>
void CLinkedList<T>::clear() {
	for (CLinkedList::iterator iter = begin(); iter != end(); ++iter) {
		iter = erase(iter);
	}
}

template<typename T>
void CLinkedList<T>::pop_front() {

	Node* node = head.child;

	head.child = node->child;
	node->child->parent = &head;

	free(node);

}

template<typename T>
void CLinkedList<T>::pop_back() {

	Node* node = tail.parent;

	tail.parent = node->parent;
	node->parent->child = &tail;

	free(node);

}

template<typename T>
typename CLinkedList<T>::iterator CLinkedList<T>::erase(CLinkedList<T>::iterator iter) {


	CLinkedList<T>::Node* node = iter.node;
	--iter;

	node->child->parent = node->parent;
	node->parent->child = node->child;

	free(node);

	return iter;

}