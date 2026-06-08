// Template: LRU Cache — Least Recently Used cache with O(1) operations
#include <iostream>
#include <unordered_map>
#include <cassert>
using namespace std;

class LRUCache {
    struct Node {
        int key, val;
        Node *prev, *next;
        Node(int k, int v) : key(k), val(v), prev(nullptr), next(nullptr) {}
    };

    unordered_map<int, Node*> cache;
    Node *head, *tail;  // dummy head and tail
    int cap_;

    // Remove node from doubly linked list
    void remove(Node* node) {
        node->prev->next = node->next;
        node->next->prev = node->prev;
    }

    // Insert node right after dummy head (most recent position)
    void addToFront(Node* node) {
        node->next = head->next;
        node->prev = head;
        head->next->prev = node;
        head->next = node;
    }

public:
    explicit LRUCache(int capacity) : cap_(capacity) {
        head = new Node(0, 0);
        tail = new Node(0, 0);
        head->next = tail;
        tail->prev = head;
    }

    ~LRUCache() {
        for (auto& [k, v] : cache) delete v;
        delete head;
        delete tail;
    }

    int get(int key) {
        auto it = cache.find(key);
        if (it == cache.end()) return -1;
        auto node = it->second;
        // Move to front (most recently used)
        remove(node);
        addToFront(node);
        return node->val;
    }

    void put(int key, int value) {
        auto it = cache.find(key);
        if (it != cache.end()) {
            // Key exists: update value and move to front
            auto node = it->second;
            node->val = value;
            remove(node);
            addToFront(node);
            return;
        }
        // Evict least recently used if at capacity
        if ((int)cache.size() == cap_) {
            auto lru = tail->prev;
            cache.erase(lru->key);
            remove(lru);
            delete lru;
        }
        auto node = new Node(key, value);
        cache[key] = node;
        addToFront(node);
    }
};

int main() {
    LRUCache lru(2);

    lru.put(1, 1);
    lru.put(2, 2);
    assert(lru.get(1) == 1);   // 1 becomes MRU; cache: [1, 2]

    lru.put(3, 3);              // evicts 2; cache: [3, 1]
    assert(lru.get(2) == -1);  // 2 was evicted
    assert(lru.get(3) == 3);   // 3 is MRU; cache: [3, 1]

    lru.put(4, 4);              // evicts 1; cache: [4, 3]
    assert(lru.get(1) == -1);  // 1 was evicted
    assert(lru.get(3) == 3);   // 3 is MRU; cache: [3, 4]
    assert(lru.get(4) == 4);   // cache: [4, 3]

    // Update existing key
    lru.put(3, 33);
    assert(lru.get(3) == 33);

    cout << "All tests passed!" << endl;
    return 0;
}
