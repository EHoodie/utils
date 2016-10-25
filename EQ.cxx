#include "EQ.h"
using namespace utils;

/** EQNode 
 */

template<typename T>
EQNode<T>::EQNode() :
	m_data(0x0),
	m_blockPtr(true) {
}

template<typename T>
EQNode<T>::EQNode(T* userData) :
	m_data((void*)userData),
	m_blockPtr(false) {
}

template<typename T>
bool EQNode<T>::isBlockPtr() const {
	return m_blockPtr;
}

template<typename T>
const EQNode<T>* EQNode<T>::getBlockPtr() const {
	return (EQNode*) m_data;
}

template<typename T>
T* EQNode<T>::getUserData() const {
	return (T*) m_data;
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

template<typename T>
EQ<T>::EQ(int blockSize) {
	m_blockSize = blockSize;
}

template<typename T>
const int EQ<T>::DEFAULT_BLOCK_SIZE = 100;
