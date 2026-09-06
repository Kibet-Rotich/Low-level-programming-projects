#include <iostream>

class NoisyBuffer
{
    int *data_;
    size_t size_;

public:
    NoisyBuffer(size_t size) : size_(size), data_(new int[size])
    {
        std::cout << "NoisyBuffer of size " << size_ << " created." << std::endl;
    }
    NoisyBuffer(const NoisyBuffer &other) : size_(other.size_), data_(new int[other.size_])
    {
        std::copy(other.data_, other.data_ + other.size_, data_);
        std::cout << "NoisyBuffer copied." << std::endl;
    }
    NoisyBuffer(NoisyBuffer &&other) noexcept : size_(other.size_), data_(other.data_)
    {
        other.data_ = nullptr;
        other.size_ = 0;
        std::cout << "NoisyBuffer moved." << std::endl;
    }

    NoisyBuffer &operator=(const NoisyBuffer &other)
    {
        std::cout << "NoisyBuffer copy assigned." << std::endl;

        // 1. Guard against self-assignment (e.g., buffer1 = buffer1;)
        if (this == &other)
        {
            return *this;
        }

        // 2. Free existing heap memory to prevent a leak
        delete[] data_;

        // 3. Allocate fresh memory and deep-copy
        size_ = other.size_;
        data_ = new int[size_];
        std::copy(other.data_, other.data_ + size_, data_);

        // 4. Return a reference to allow chained assignments (a = b = c)
        return *this;
    }
    NoisyBuffer &operator=(NoisyBuffer &&other) noexcept
    {
        std::cout << "NoisyBuffer move assigned." << std::endl;

        // 1. Guard against self-move (e.g., buffer1 = std::move(buffer1);)
        if (this == &other)
        {
            return *this;
        }

        // 2. Free existing heap memory
        delete[] data_;

        // 3. Steal resources
        data_ = other.data_;
        size_ = other.size_;

        // 4. Neutralize the source
        other.data_ = nullptr;
        other.size_ = 0;

        return *this;
    }
    ~NoisyBuffer()
    {
        delete[] data_;
        std::cout << "NoisyBuffer of size " << size_ << " destroyed." << std::endl;
    }
    void fill(int value)
    {
        for (size_t i = 0; i < size_; ++i)
        {
            data_[i] = value;
        }
    }
    void print() const
    {
        for (size_t i = 0; i < size_; ++i)
        {
            std::cout << data_[i] << " ";
        }
        std::cout << std::endl;
    }
};

int main()
{
    NoisyBuffer buffer1(5);
    buffer1.fill(42);
    buffer1.print();

    NoisyBuffer buffer2 = buffer1; // Copy constructor
    buffer2.print();

    NoisyBuffer buffer3 = std::move(buffer1); // Move constructor
    buffer3.print();

    return 0;
}