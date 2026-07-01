#include <iostream>
#include <chrono>
#include <vector>
#include <cstddef>
#include <new>

// The 32-byte target object
struct Node {
    long long data[3];
    Node* next;
};

// Your Custom Allocator
class Arena {
private:
    std::byte* buffer;
    std::size_t capacity;
    std::size_t offset;

public:
    explicit Arena(std::size_t capacity) : capacity(capacity), offset(0) {
        buffer = new std::byte[capacity];
    }
    
    ~Arena() {
        delete[] buffer;
    }

    void* allocate(std::size_t size) {
        if (offset + size > capacity) {
            throw std::bad_alloc();
        }
        void* ptr = buffer + offset;
        offset += size;
        return ptr;
    }
};

int main() {
    const int ITERATIONS = 1'000'000;
    const int TRIES = 10;
    
    long long total_os_time = 0;
    long long total_arena_time = 0;

    std::cout << "Running " << TRIES << " trials of " << ITERATIONS << " allocations...\n\n";

    for (int t = 0; t < TRIES; ++t) {
        
        // --- 1. OS ALLOCATOR (new/delete) ---
        std::vector<Node*> os_pointers;
        os_pointers.reserve(ITERATIONS); 
        
        auto start_os = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < ITERATIONS; ++i) {
            os_pointers.push_back(new Node());
        }
        for (int i = 0; i < ITERATIONS; ++i) {
            delete os_pointers[i];
        }
        auto end_os = std::chrono::high_resolution_clock::now();
        total_os_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_os - start_os).count();

        // --- 2. ARENA ALLOCATOR ---
        std::vector<Node*> arena_pointers;
        arena_pointers.reserve(ITERATIONS);
        
        auto start_arena = std::chrono::high_resolution_clock::now();
        {
            // We need enough bytes to hold 1,000,000 Nodes
            Arena my_arena(ITERATIONS * sizeof(Node));
            
            for (int i = 0; i < ITERATIONS; ++i) {
                // Placement new
                arena_pointers.push_back(new (my_arena.allocate(sizeof(Node))) Node());
            }
        } // my_arena goes out of scope here. The  slab is destroyed instantly.
        auto end_arena = std::chrono::high_resolution_clock::now();
        total_arena_time += std::chrono::duration_cast<std::chrono::milliseconds>(end_arena - start_arena).count();
    }

    // --- REPORTING ---
    double avg_os = (double)total_os_time / TRIES;
    double avg_arena = (double)total_arena_time / TRIES;

    std::cout << "--- RESULTS (Average over " << TRIES << " tries) ---\n";
    std::cout << "Standard OS (new/delete): " << avg_os << " ms\n";
    std::cout << "Arena Allocator:          " << avg_arena << " ms\n\n";
    
    if (avg_arena > 0) {
        std::cout << "Arena is " << (avg_os / avg_arena) << "x faster.\n";
    }

    return 0;
}