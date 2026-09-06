#include <iostream>
#include <vector>

void processData( std::vector<int>& data) {
    for (auto& value : data) {
        // Perform some processing on each value
        value *= 2; // Example processing: doubling the value
    }
}


int main() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    
    for(const auto& value : data) {
        std::cout << "Original value: " << value << std::endl;
    }
    processData(data);
    for(const auto& value : data) {
        std::cout << "Final value: " << value << std::endl;
    }
    return 0;
}