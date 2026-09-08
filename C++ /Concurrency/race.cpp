#include <iostream>
#include <vector>
#include <thread>
#include <mutex>


std::mutex mtx; // Mutex for synchronizing access to shared_counter

int shared_counter = 0;

void incrementCounter() {
    for (int i = 0; i < 100000; ++i) {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex to protect shared_counter
        shared_counter++; // <--- THE DATA RACE
    }
}

void optimizedIncrementCounter() {
    int local_counter = 0;
    for (int i = 0; i < 100000; ++i) {
        local_counter++;
    }
    {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex to protect shared_counter
        shared_counter += local_counter; // Update the shared counter once
    }
}

int main() {
    std::cout << "Spawning threads...\n";
    
    {
        std::vector<std::jthread> workers;
        for (int i = 0; i < 10; ++i) {
            workers.emplace_back(optimizedIncrementCounter);
        }
    } // All 10 threads are guaranteed to be joined and finished here!

    std::cout << "Expected: 1000000\n";
    std::cout << "Actual:   " << shared_counter << "\n";
    
    return 0;
}