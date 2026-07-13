#include <iostream>
#include <chrono>
#include <vector>
#include <thread>
#include <cmath>

// A simulated heavy workload (e.g., Matrix math, ML inference step)
void heavy_task() {
    // 'volatile' forces the compiler to actually do the math 
    // instead of optimizing the loop away entirely.
    volatile double result = 0; 
    for (int i = 0; i < 10000; ++i) {
        result += std::sin(i) * std::cos(i);
    }
}

int main() {
    const int NUM_TASKS = 10'000;

    std::cout << "Benchmarking " << NUM_TASKS << " heavy tasks...\n\n";

    // --- 1. SEQUENTIAL EXECUTION (1 Core, 100% Utilization) ---
    auto start_seq = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < NUM_TASKS; ++i) {
        heavy_task();
    }
    auto end_seq = std::chrono::high_resolution_clock::now();
    auto time_seq = std::chrono::duration_cast<std::chrono::milliseconds>(end_seq - start_seq).count();
    std::cout << "[Core 0] Sequential Time:   " << time_seq << " ms\n";

    // --- 2. NAIVE MULTITHREADING (Spawn a thread per task) ---
    // This forces the OS kernel to build and tear down 10,000 separate thread contexts.
    auto start_naive = std::chrono::high_resolution_clock::now();
    std::vector<std::thread> threads;
    threads.reserve(NUM_TASKS);

    for (int i = 0; i < NUM_TASKS; ++i) {
        threads.emplace_back(heavy_task);
    }

    // Wait for all 10,000 threads to finish
    for (auto& t : threads) {
        t.join();
    }
    auto end_naive = std::chrono::high_resolution_clock::now();
    auto time_naive = std::chrono::duration_cast<std::chrono::milliseconds>(end_naive - start_naive).count();
    std::cout << "[Kernel] Naive std::thread: " << time_naive << " ms\n";

    return 0;
}