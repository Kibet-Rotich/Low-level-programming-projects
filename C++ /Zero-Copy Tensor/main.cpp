#include <iostream>
#include <memory>
#include <span>
#include <concepts>
#include <type_traits>
#include <utility> // For std::exchange and std::move

// 1. C++20 CONCEPTS: Strict Compile-Time Type Gating
// We define a Concept named 'Numeric'. It evaluates to true only if T is an int, float, double, etc.
// If someone tries to create a Tensor<std::string>, the compiler stops immediately with a clean error.
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// 2. TEMPLATES: The 'Numeric' concept replaces 'typename' here.
template <Numeric T>
class Tensor {
private:
    size_t size_;
    
    // 3. SMART POINTERS: unique_ptr guarantees zero memory leaks.
    // It owns a dynamically allocated array (T[]). When the Tensor dies, 
    // the unique_ptr automatically calls delete[] on the data.
    std::unique_ptr<T[]> data_;

public:
    // 4. COMPILE-TIME COMPUTING: constexpr
    // Because this function is constexpr, if you pass hardcoded numbers (like 2, 3), 
    // the compiler calculates '6' during compilation. No math happens at runtime.
    static constexpr size_t compute_size(size_t rows, size_t cols) {
        return rows * cols;
    }

    // 5. CONSTRUCTOR & RAII
    // We use std::make_unique to allocate the heap memory safely.
    Tensor(size_t rows, size_t cols) 
        : size_(compute_size(rows, cols)), 
          data_(std::make_unique<T[]>(size_)) {
        std::cout << "Allocated Tensor of size " << size_ << ".\n";
    }

    // 6. THE RULE OF 5: Deleting Copies
    // We physically prevent the compiler from generating copy operations. 
    // If you try `Tensor t2 = t1;`, compilation fails. This absolutely guarantees 
    // you never accidentally deep-copy a massive multi-gigabyte Tensor.
    Tensor(const Tensor& other) = delete;
    Tensor& operator=(const Tensor& other) = delete;

    // 7. THE RULE OF 5: Move Constructor
    // Takes an rvalue reference (&&). It steals the size and the unique_ptr.
    Tensor(Tensor&& other) noexcept 
        : size_(other.size_), 
          // unique_ptr cannot be copied, but it CAN be moved. std::move forces the transfer.
          data_(std::move(other.data_)) {
        
        // We must neutralize the source object's size so it knows it is empty.
        // (unique_ptr automatically nulls itself out when moved, so we don't need to do data_ = nullptr).
        other.size_ = 0;
        std::cout << "Moved Tensor via constructor.\n";
    }

    // 8. THE RULE OF 5: Move Assignment Operator
    // Handles assigning an rvalue to an ALREADY EXISTING Tensor.
    Tensor& operator=(Tensor&& other) noexcept {
        // Always guard against self-assignment (e.g., t1 = std::move(t1))
        if (this != &other) {
            size_ = other.size_;
            
            // When we move assign a unique_ptr, it automatically deletes our OLD memory first,
            // then steals the memory from 'other', and then nulls out 'other'. It's magic.
            data_ = std::move(other.data_);
            
            other.size_ = 0;
            std::cout << "Moved Tensor via assignment.\n";
        }
        return *this;
    }

    // 9. C++20 SPANS: Zero-Cost Views
    // Returns a lightweight span instead of a new vector. 
    // data_.get() retrieves the raw pointer from the unique_ptr so the span can observe it.
    std::span<T> slice(size_t start, size_t length) {
        if (start + length > size_) {
            throw std::out_of_range("Slice exceeds Tensor bounds!");
        }
        return std::span<T>(data_.get() + start, length);
    }

    // Utility methods
    void fill(T val) {
        for(size_t i = 0; i < size_; ++i) data_[i] = val;
    }
    
    void print() const {
        for(size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << "\n";
    }
    
    size_t size() const { return size_; }
};

int main() {
    // 1. Create a 2x3 float tensor
    std::cout << "--- Initialization ---\n";
    Tensor<float> t1(2, 3);
    t1.fill(1.5f);
    std::cout << "t1: "; t1.print();

    // 2. Test the Move Constructor
    std::cout << "\n--- Move Semantics ---\n";
    Tensor<float> t2 = std::move(t1);
    std::cout << "t2: "; t2.print();
    std::cout << "t1 size after move: " << t1.size() << " (Should be 0)\n";

    // 3. Test Zero-Cost Slicing
    std::cout << "\n--- std::span Slicing ---\n";
    // We slice from index 3, grabbing 3 elements (effectively the second row of a 2x3 matrix)
    std::span<float> row_view = t2.slice(3, 3); 
    
    std::cout << "View of 2nd row: ";
    for(float val : row_view) {
        std::cout << val << " ";
    }
    std::cout << "\n";

    // 4. Prove the view modifies the original memory (Zero-Copy)
    row_view[0] = 99.9f; 
    std::cout << "t2 after modifying view: "; 
    t2.print(); // You will see 99.9 where 1.5 used to be!

    return 0; // t2 falls out of scope, unique_ptr deletes the heap memory cleanly.
}