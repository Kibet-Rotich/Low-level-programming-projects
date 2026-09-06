#include <iostream>
#include <chrono>

class ScopeTimer {
public:
    ScopeTimer(const std::string& name) : name_(name), start_time_(std::chrono::high_resolution_clock::now()) {}
    ~ScopeTimer() {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_).count();
        std::cout << "ScopeTimer [" << name_ << "] duration: " << duration << " microseconds" << std::endl;
    }
private:
    std::string name_;
    std::chrono::high_resolution_clock::time_point start_time_;
};


int main() {
    {
        ScopeTimer timer("Example Scope");
        // Simulate some work
        for (volatile int i = 0; i < 1000000; ++i);
    } // The timer will automatically stop and report duration here

    return 0;
}
