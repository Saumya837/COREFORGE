#include <atomic>
#include <pthread.h>

namespace CoreThread{
    class SpinLock{
        private:
            std::atomic_flag flag = ATOMIC_FLAG_INIT;
        public:
            void lock();
            void unlock();
    };
}
