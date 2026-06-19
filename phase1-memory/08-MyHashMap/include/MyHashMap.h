#include <vector>
#include <functional>
#include <utility>  // std::pair

template<typename K, typename V>
class HashMap {
private:
    struct Node {
        std::pair<const K, V> data;  // STL pair<key,value> jaisa
        Node* next;

        Node(const K& k, const V& v) : data(k, v), next(nullptr) {}
    };

    std::vector<Node*> buckets;
    size_t bucket_count;
    size_t element_count;

    size_t hashKey(const K& key) const {
        return std::hash<K>{}(key) % bucket_count;
    }

public:
    // ===== ITERATOR CLASS =====
    class iterator {
    private:
        HashMap* map;        // parent hashmap ka pointer
        size_t bucket_idx;   // kis bucket mein hai
        Node* current;       // kis node par hai

        // Helper: agar current bucket khatam ho gaya, next non-empty bucket dhundo
        void advanceToValidBucket() {
            // TODO:
            // jab tak current == nullptr aur bucket_idx < bucket_count
            //   bucket_idx++ karo
            //   current = map->buckets[bucket_idx] try karo
        }

    public:
        iterator(HashMap* m, size_t idx, Node* node)
            : map(m), bucket_idx(idx), current(node) {}

        // Dereference — *it karne par pair milna chahiye
        std::pair<const K, V>& operator*() {
            return current->data;
        }

        std::pair<const K, V>* operator->() {
            return &(current->data);
        }

        // Prefix increment — ++it
        iterator& operator++() {
            // TODO:
            // 1. current = current->next (linked list mein aage)
            // 2. agar current nullptr ho gaya -> advanceToValidBucket() call karo
            return *this;
        }

        bool operator==(const iterator& other) const {
            return current == other.current;
        }

        bool operator!=(const iterator& other) const {
            return current != other.current;
        }
    };

    // ===== CONSTRUCTOR =====
    HashMap(size_t initial_buckets = 16)
        : bucket_count(initial_buckets), element_count(0) {
        buckets.resize(bucket_count, nullptr);
    }

    // ===== begin() / end() =====
    iterator begin() {
        // TODO:
        // bucket 0 se shuru karke pehla non-empty bucket dhundo
        // agar koi node nahi mila -> end() return karo
    }

    iterator end() {
        // TODO: ek "past the end" iterator return karo
        // current = nullptr, bucket_idx = bucket_count
        return iterator(this, bucket_count, nullptr);
    }

    // ===== operator[] =====
    V& operator[](const K& key) {
        // TODO:
        // 1. key dhundo bucket mein
        // 2. mil gayi -> reference return karo value ki
        // 3. nahi mili -> naya node banao value default-constructed,
        //    insert karo, reference return karo
    }

    // ===== find() — iterator return karta hai =====
    iterator find(const K& key) {
        // TODO:
        // 1. hashKey() se bucket index nikalo
        // 2. us bucket mein traverse karo
        // 3. mil gayi -> iterator(this, idx, node) return karo
        // 4. nahi mili -> end() return karo
    }

    // ===== insert() =====
    void insert(const std::pair<K, V>& kv) {
        operator[](kv.first) = kv.second;
    }

    // ===== erase() =====
    bool erase(const K& key) {
        // TODO: same as pehle — prev pointer track karke remove karo
    }

    size_t size() const { return element_count; }
    size_t count(const K& key) {
        return find(key) != end() ? 1 : 0;
    }

    ~HashMap() {
        // TODO: har bucket free karo
    }

    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;
};