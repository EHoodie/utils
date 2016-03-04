#ifndef _E_TREE_H_ 
#define _E_TREE_H_ 

#include "EResult.h"
#include "ESpinLock.h"

namespace utils {

	template<typename T>
	class ETree {

		public:
			EResult put(T value);
			bool contains(T value) const;

		private:
	
			struct ETreeNode {
				T value;
				ETreeNode* left;
				ETreeNode* right;
			};
			

	}; // class ETree

} // namespace

#endif // define 
