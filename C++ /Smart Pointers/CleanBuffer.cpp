#include <iostream>
#include <memory>

class CleanBuffer {
    size_t  size_;
    std::unique_ptr<int[]> data_;

    public:
    CleanBuffer(size_t size) : size_(size), data_(std::make_unique<int[]>(size)) {
        std::cout << "CleanBuffer of size " << size_ << " created." << std::endl;
    }

    void fill(int value) {
        for (size_t i = 0; i < size_; ++i) {
            data_[i] = value;
        }
    }
    void print() const {
        for (size_t i = 0; i < size_; ++i) {
            std::cout << data_[i] << " ";
        }
        std::cout << std::endl;
    }
};

int main() {
    CleanBuffer buffer(5);
    buffer.fill(42);
    buffer.print();

    CleanBuffer buffer2 = std::move(buffer); // Move constructor
    buffer2.print();

    return 0;
}