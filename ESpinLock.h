#ifndef _E_SPIN_LOCK_H_
#define _E_SPIN_LOCK_H_

namespace utils {

	class ESpinLock {

		public:
			bool tryLock(void);
			bool lock(void);
			bool unlock(void);

		private:
			bool m_lock;

	}; // class

} //namespace

#endif // define
