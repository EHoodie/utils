#include "EQ.h"
#include "InvalidException.h"

using namespace utils;

/** EQNode 
 */

template<typename T>
EQNode<T>::EQNode() :
	m_data(0x0),
	m_blockPtr(true) {
}

template<typename T>
EQNode<T>::EQNode(EQNode<T>* blockPtr) :
	m_data((void*)blockPtr),
	m_blockPtr(true) {
}

template<typename T>
EQNode<T>::EQNode(T* userData) :
	m_data((void*)userData),
	m_blockPtr(false) {
}

template<typename T>
bool EQNode<T>::isFree() const {
	return (m_data == 0x0);
}

template<typename T>
bool EQNode<T>::isBlockPtr() const {
	return m_blockPtr;
}

template<typename T>
const EQNode<T>* EQNode<T>::getBlockPtr() const {
	if (isBlockPtr() == false) {
		throw InvalidException("EQNode<T>::getBlockPtr: Node is not a block pointer");
	}
	return (EQNode<T>*) m_data;
}

template<typename T>
bool EQNode<T>::setBlockPtr(EQNode<T>* blockPtr) {
	if (isBlockPtr() == false) {
		throw InvalidException("EQNode<T>::getUserData: Node is not a block pointer");
	}

	// multiple threads can try to take this node at the same time
	return __sync_bool_compare_and_swap(&m_data, 0x0, (void*) blockPtr);
}

template<typename T>
T* EQNode<T>::getUserData() const {
	if (isBlockPtr() == true) {
		throw InvalidException("EQNode<T>::getUserData: Node is a block pointer");
	}

	return (T*) m_data;
}

template<typename T>
bool EQNode<T>::setUserData(T* userData) {
	if (isBlockPtr() == true) {
		throw InvalidException("EQNode<T>::getUserData: Node is a block pointer");
	}

	// multiple threads can try to take this node at the same time
	return __sync_bool_compare_and_swap(&m_data, 0x0, (void*) userData);
}

template<typename T>
EQNode<T>::~EQNode() {
	if (isBlockPtr() == false) {
		T* data = (T*) m_data;
		delete data;
	}
}

/** EQ
 */

// public

template<typename T>
const int EQ<T>::DEFAULT_BLOCK_SIZE = 100;

template<typename T>
EQ<T>::EQ(int blockSize) {
	m_blockSize = blockSize;
}

template<typename T>
int EQ<T>::enqueue(T* userData) {
	bool success = false;

	do {
		EQNode<T>* node = findFreeNode();
		bool success = node->setUserData(userData);
	} while (success == false);

	return 0;
}

template<typename T>
T* EQ<T>::dequeue() {

}

template<typename T>
long EQ<T>::size() const {
	return m_size;
}

// private

template<typename T>
EQNode<T>* EQ<T>::findFreeNode() const {
	// create new block if there aren't any
	if (m_tail == 0x0) {
		createInitialBlock();
	}

	// a block exists, find the first free node in the block
	EQNode<T>* myTail = m_tail;
	EQNode<T>* node = findFreeNodeInBLock(myTail);
	while (node == 0x0) {
		// note the current tail so that only one thread adds a new block at a time
		addNextBlock(myTail);

		myTail = m_tail;
		node = findFreeNodeInBLock(myTail);
	}

	return node;
}

template<typename T>
EQNode<T>* EQ<T>::findFreeNodeInBlock(EQNode<T>* tailPtr) const {
	if (tailPtr != m_tail) {
		// another thread added a block
		return 0x0;
	}

	EQNode<T>* node = 0x0;

	// only walk until block size - 2, since last node will be a block ptr
	for (int x = 0; x <= m_blockSize - 2; x ++) {
		node = tailPtr[x];

		if (node->isFree() == true) {
			return node;
		}
	}

	return 0x0;
}

template<typename T>
void EQ<T>::addNextBlock(EQNode<T>* tailPtr) {
	if (tailPtr != m_tail) {
		// another thread added a block
		return;
	}

	EQNode<T>* blockPtr = tailPtr[m_blockSize - 1];
	EQNode<T>* nextBlock = createBlock();

	bool success = blockPtr->setBlockPtr(nextBlock);
	if (success == false) {
		// another thread added a block
		delete [] nextBlock;
	} else {
		m_tail = nextBlock;
	}
}

template<typename T>
void EQ<T>::createInitialBlock() {
	if (m_tail == 0x0) {
		EQNode<T>* nextBlock = createBlock();
		EQNode<T>* nextBlockPtr = createBlockPtrNode(nextBlock);

		if (__sync_bool_compare_and_swap(&m_head, 0x0, nextBlockPtr) == true) {
			// using this thread's new block	
			m_tail = m_head;
		} else {
			// another thread got in first and created a head
			delete [] nextBlock;
			delete nextBlockPtr;
		}
	}
}

template<typename T>
EQNode<T>* EQ<T>::createBlock() const {
	return new EQNode<T>[m_blockSize];
}

template<typename T>
EQNode<T>* EQ<T>::createBlockPtrNode(EQNode<T>* blockPtr) const {
	return new EQNode<T>(blockPtr, true /*blockPtr */);
}


