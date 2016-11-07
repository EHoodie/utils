/** Eric Mann - a small, semi-dynamic, highly concurrent, lockless queue implementation
 */

namespace utils {

/** a node representing either user data, or a link to the next block
 */
template<typename T>
class EQNode {
	public:
		EQNode();
		EQNode(EQNode<T>* blockPtr);
		EQNode(T* userData);

		bool isFree() const;
		bool isBlockPtr() const;
		const EQNode<T>* getBlockPtr() const;
		bool setBlockPtr(EQNode<T>* blockPtr);
		T* getUserData() const;
		bool setUserData(T* userData);

		virtual ~EQNode();

	private:
		void* m_data;
		const bool m_blockPtr:1;

}; // class EQNode

template<typename T>
class EQ {
	public:
		const static int DEFAULT_BLOCK_SIZE;
	public:
		/** create an EQ with the specified block size
		 */
		EQ(int blockSize = DEFAULT_BLOCK_SIZE);

		/** add userData to the queue. Returns 0 on success.
		 */
		int enqueue(T* userData);

		/** dequeue userData from the queue. Returns userData on success, null on failure.
		 */
		T* dequeue();

		/** return the number of items currently in the queue
		 */
		long size() const;

		virtual ~EQ();

	private:
		EQNode<T>* findFreeNode() const;
		EQNode<T>* findFreeNodeInBlock(EQNode<T>* tailPtr) const;
		void addNextBlock(EQNode<T>* tailPtr);
		void createInitialBlock();

		EQNode<T>* createBlock() const;
		EQNode<T>* createBlockPtrNode(EQNode<T>* blockPtr) const;

	private:
		long m_size;
		EQNode<T>* m_head;
		EQNode<T>* m_tail;
		int m_blockSize;

}; // class EQ

} // namespace utils
