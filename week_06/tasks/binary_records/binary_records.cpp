#include <cstdint>
#include <stdexcept>
#include <string>


struct Student {
    std::uint32_t id = 0;
    std::string name;
    std::int32_t score = 0;

    bool operator==(const Student&) const = default;
};

/* return_type */ WriteStudents(/* args */) {
    throw std::runtime_error{"Not implemented"};
}

/* return_type */ ReadStudents(/* args */) {
    throw std::runtime_error{"Not implemented"};
}

/* return_type */ CountStudents(/* args */) {
    throw std::runtime_error{"Not implemented"};
}

/* return_type */ UpdateScore(/* args */) {
    throw std::runtime_error{"Not implemented"};
}
