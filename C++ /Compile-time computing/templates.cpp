#include<iostream>
#include<cstring>

template<typename T>
T computeSquare(T value) {
    return value * value;
}

template<typename T>
struct Point {
    T x;
    T y;

    Point(T x_val, T y_val) : x(x_val), y(y_val) {}

    void display() const {
        std::cout << "Point(" << x << ", " << y << ")" << std::endl;
    }
};



template<typename T>
T computeMax(T a, T b) {
    return (a > b) ? a : b;
}
template<>
const char* computeMax<const char*>(const char* a, const char* b) {
    return (std::strcmp(a, b) > 0) ? a : b;
}

int main() {
    int intValue = 5;
    double doubleValue = 3.14;

    std::cout << "Square of " << intValue << " is " << computeSquare(intValue) << std::endl;
    std::cout << "Square of " << doubleValue << " is " << computeSquare(doubleValue) << std::endl;

    Point<int> intPoint(1, 2);
    Point<double> doublePoint(3.5, 4.5);
    intPoint.display();
    doublePoint.display();

    return 0;
}