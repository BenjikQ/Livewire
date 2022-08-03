#include <iostream>

#include <boost/heap/fibonacci_heap.hpp>

int main() {
    boost::heap::fibonacci_heap<int> heap{};

    heap.push(1);
    heap.push(2);
    heap.push(3);

    for (auto el : heap)
        std::cout << el << '\n';
}
