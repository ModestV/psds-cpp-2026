#include <cstdint>
#include <iostream>


enum class CheckFlags : uint8_t {
    NONE = 0,
    TIME = (1 << 0),
    DATE = (1 << 1),
    USER = (1 << 2),
    CERT = (1 << 3),
    KEYS = (1 << 4),
    DEST = (1 << 5),
    ALL = TIME | DATE | USER | CERT | KEYS | DEST
};


void PrintCheckFlags(CheckFlags flags) {
    const uint8_t value = static_cast<uint8_t>(flags);
    const uint8_t all_flags = static_cast<uint8_t>(CheckFlags::ALL);

    // Проверяем, что нет неизвестных битов.
    if ((value & ~all_flags) != 0) {
        return;
    }

    std::cout << "[";

    if (value == 0) {
        std::cout << "]";
        return;
    }

    bool first = true;

    if (value & static_cast<uint8_t>(CheckFlags::TIME)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "TIME";
        first = false;
    }

    if (value & static_cast<uint8_t>(CheckFlags::DATE)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "DATE";
        first = false;
    }

    if (value & static_cast<uint8_t>(CheckFlags::USER)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "USER";
        first = false;
    }

    if (value & static_cast<uint8_t>(CheckFlags::CERT)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "CERT";
        first = false;
    }

    if (value & static_cast<uint8_t>(CheckFlags::KEYS)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "KEYS";
        first = false;
    }

    if (value & static_cast<uint8_t>(CheckFlags::DEST)) {
        if (!first) {
            std::cout << ",";
        }
        std::cout << "DEST";
    }

    std::cout << "]";
}
