#include <iostream>
#include <chrono>
#include <vector>

// A standard 32-byte object (typical for trees or game entities)
struct Node {
    long long data[3];
    Node* next;
};

int main() {
    const int ITERATIONS = 1'000'000;
    
    // We reserve the vector upfront so we strictly benchmark the allocator,
    // not the vector dynamically resizing itself.
    std::vector<Node*> pointers;
    pointers.reserve(ITERATIONS); 

    std::cout << "Benchmarking standard OS allocator (new/delete)...\n";

    auto start = std::chrono::high_resolution_clock::now();

    // The Allocation Phase
    for (int i = 0; i < ITERATIONS; ++i) {
        pointers.push_back(new Node());
    }

    // The Deallocation Phase
    for (int i = 0; i < ITERATIONS; ++i) {
        delete pointers[i];
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Total Time: " << duration << " ms\n";

    return 0;
}