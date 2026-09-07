#include <iostream>
#include <array>

// constexpr: Can run at compile-time OR runtime
constexpr size_t computeTensorFlatSize(size_t batch, size_t channels, size_t height, size_t width) {
    return batch * channels * height * width;
}

// consteval: FORCED to run at compile time (C++20)
consteval size_t alignToCacheLine(size_t bytes) {
    constexpr size_t cache_line = 64;
    return (bytes + cache_line - 1) & ~(cache_line - 1);
}

int main() {
    // 1. Used to size a stack array at compile time:
    constexpr size_t flat_size = computeTensorFlatSize(32, 3, 224, 224);
    std::cout << "Flat elements: " << flat_size << "\n";

    // 2. consteval enforces compile-time alignment calculation
    constexpr size_t aligned_buffer_size = alignToCacheLine(flat_size * sizeof(float));
    std::cout << "Cache-aligned bytes: " << aligned_buffer_size << "\n";

    // 3. consteval fails if inputs are dynamic:
    size_t runtime_val = 120;
    // size_t bad = alignToCacheLine(runtime_val); 
    // ^ COMPILE ERROR: 'runtime_val' is not a constant expression!
}