#include <cstddef>
#include <iostream>


void PrintBits(long long value, size_t bytes) {
    if (bytes == 0 || bytes > sizeof(long long)) {
        return;
    }

    const unsigned long long number =
        static_cast<unsigned long long>(value);

    std::cout << "0b";

    const size_t bits = bytes * 8;

    for (size_t i = bits; i > 0; --i) {
        const size_t bit = i - 1;

        std::cout << ((number >> bit) & 1);

        if (bit > 0 && bit % 4 == 0) {
            std::cout << "'";
        }
    }

    std::cout << '\n';
}
