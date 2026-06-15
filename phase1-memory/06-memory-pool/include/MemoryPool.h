#include <memory>
#include <cstddef>

template<typename T>
class memoryPool{
    private:
        struct Block{
            alignas(T) unsigned char data[sizeof(T)]; // T ka storage
            Block* _next_free;                          // free list link
        };

    private:
        static const int _numBlocks = 32; 
        std::allocator<Block> _alloc;
        Block* _pool;
        Block* _free_head;

    public:
        memoryPool(): _alloc{}, _pool(nullptr), _free_head(nullptr){
            _pool = _alloc.allocate(_numBlocks);
            
            for(int i = 0; i < _numBlocks - 1; i++){
                _pool[i]._next_free = &_pool[i+1];
            }

            _pool[_numBlocks - 1]._next_free = nullptr;
            _free_head = &_pool[0];
        }

        T* allocate(){
            if(!_free_head) return nullptr;

            Block* block = _free_head;
            _free_head = _free_head->_next_free;
            return reinterpret_cast<T*>(block->data); 
        }

        void deallocate(T* ptr){
            Block* block = reinterpret_cast<Block*>(ptr); 
            block->_next_free = _free_head;
            _free_head = block;
        }

        ~memoryPool() {
            _alloc.deallocate(_pool, _numBlocks);
        }

        // Copy band karo (pool copy nahi honi chahiye)
        memoryPool(const memoryPool&) = delete;
        memoryPool& operator=(const memoryPool&) = delete;
};
