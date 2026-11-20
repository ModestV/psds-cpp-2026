#include <cstdint>
#include <istream>
#include <limits>
#include <ostream>
#include <string>
#include <vector>
#include <iostream>
#include <utility>

struct Student {
    std::uint32_t id = 0;
    std::string name;
    std::int32_t score = 0;

    bool operator==(const Student&) const = default;
};

namespace binary_records_detail {

constexpr char kMagic[4] = {'P', 'S', 'D', 'S'};
constexpr std::uint16_t kVersion = 1;

bool ReadExact(std::istream& input, char* data, std::streamsize size) {
    if (size == 0) {
        return true;
    }
    input.read(data, size);
    return input.gcount() == size;
}

bool WriteExact(std::ostream& output, const char* data, std::streamsize size) {
    if (size == 0) {
        return true;
    }
    output.write(data, size);
    return output.good();
}

bool ReadU16(std::istream& input, std::uint16_t& value) {
    unsigned char bytes[2]{};
    if (!ReadExact(input, reinterpret_cast<char*>(bytes), 2)) {
        return false;
    }

    value = static_cast<std::uint16_t>(bytes[0]) |
            (static_cast<std::uint16_t>(bytes[1]) << 8);

    return true;
}

bool ReadU32(std::istream& input, std::uint32_t& value) {
    unsigned char bytes[4]{};
    if (!ReadExact(input, reinterpret_cast<char*>(bytes), 4)) {
        return false;
    }

    value = static_cast<std::uint32_t>(bytes[0]) |
            (static_cast<std::uint32_t>(bytes[1]) << 8) |
            (static_cast<std::uint32_t>(bytes[2]) << 16) |
            (static_cast<std::uint32_t>(bytes[3]) << 24);

    return true;
}

bool WriteU16(std::ostream& output, std::uint16_t value) {
    const char bytes[2] = {
        static_cast<char>(value & 0xFFu),
        static_cast<char>((value >> 8) & 0xFFu),
    };

    return WriteExact(output, bytes, 2);
}

bool WriteU32(std::ostream& output, std::uint32_t value) {
    const char bytes[4] = {
        static_cast<char>(value & 0xFFu),
        static_cast<char>((value >> 8) & 0xFFu),
        static_cast<char>((value >> 16) & 0xFFu),
        static_cast<char>((value >> 24) & 0xFFu),
    };

    return WriteExact(output, bytes, 4);
}

bool ReadHeader(std::istream& input, std::uint32_t& count) {
    char magic[4]{};

    if (!ReadExact(input, magic, 4)) {
        return false;
    }

    if (magic[0] != kMagic[0] || magic[1] != kMagic[1] ||
        magic[2] != kMagic[2] || magic[3] != kMagic[3]) {
        return false;
    }

    std::uint16_t version = 0;

    if (!ReadU16(input, version) || version != kVersion) {
        return false;
    }

    return ReadU32(input, count);
}

bool WriteHeader(std::ostream& output, std::uint32_t count) {
    return WriteExact(output, kMagic, 4) &&
           WriteU16(output, kVersion) &&
           WriteU32(output, count);
}

}  // namespace binary_records_detail

bool WriteStudents(std::ostream& output, const std::vector<Student>& students) {
    if (!output.good() ||
        students.size() > std::numeric_limits<std::uint32_t>::max()) {
        return false;
    }

    for (const Student& student : students) {
        if (student.name.size() > std::numeric_limits<std::uint16_t>::max()) {
            return false;
        }
    }

    if (!binary_records_detail::WriteHeader(
            output, static_cast<std::uint32_t>(students.size()))) {
        return false;
    }

    for (const Student& student : students) {
        if (!binary_records_detail::WriteU32(output, student.id) ||
            !binary_records_detail::WriteU16(
                output, static_cast<std::uint16_t>(student.name.size())) ||
            !binary_records_detail::WriteExact(
                output, student.name.data(),
                static_cast<std::streamsize>(student.name.size())) ||
            !binary_records_detail::WriteU32(
                output, static_cast<std::uint32_t>(student.score))) {
            return false;
        }
    }

    return output.good();
}

bool ReadStudents(std::istream& input, std::vector<Student>& students) {
    std::uint32_t count = 0;

    if (!binary_records_detail::ReadHeader(input, count)) {
        return false;
    }

    std::vector<Student> loaded;

    for (std::uint32_t i = 0; i < count; ++i) {
        Student student;
        std::uint16_t name_length = 0;

        if (!binary_records_detail::ReadU32(input, student.id) ||
            !binary_records_detail::ReadU16(input, name_length)) {
            return false;
        }

        student.name.resize(name_length);

        if (!binary_records_detail::ReadExact(
                input, student.name.data(),
                static_cast<std::streamsize>(name_length))) {
            return false;
        }

        std::uint32_t raw_score = 0;

        if (!binary_records_detail::ReadU32(input, raw_score)) {
            return false;
        }

        student.score = static_cast<std::int32_t>(raw_score);
        loaded.push_back(std::move(student));
    }

    char extra = 0;
    input.read(&extra, 1);

    if (input.gcount() != 0 || !input.eof() || input.bad()) {
        return false;
    }

    students = std::move(loaded);
    return true;
}

std::int64_t CountStudents(std::istream& input) {
    std::uint32_t count = 0;

    if (!binary_records_detail::ReadHeader(input, count)) {
        return -1;
    }

    return static_cast<std::int64_t>(count);
}

bool UpdateScore(std::iostream& stream,
                 std::uint32_t id,
                 std::int32_t score) {
    stream.clear();
    stream.seekg(0, std::ios::beg);

    if (!stream.good()) {
        return false;
    }

    std::uint32_t count = 0;

    if (!binary_records_detail::ReadHeader(stream, count)) {
        return false;
    }

    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t current_id = 0;
        std::uint16_t name_length = 0;

        if (!binary_records_detail::ReadU32(stream, current_id) ||
            !binary_records_detail::ReadU16(stream, name_length)) {
            return false;
        }

        stream.seekg(static_cast<std::streamoff>(name_length), std::ios::cur);

        if (!stream.good()) {
            return false;
        }

        const std::streampos score_position = stream.tellg();

        if (score_position == std::streampos(-1)) {
            return false;
        }

        std::uint32_t old_score = 0;

        if (!binary_records_detail::ReadU32(stream, old_score)) {
            return false;
        }

        if (current_id == id) {
            stream.clear();
            stream.seekp(score_position);

            if (!stream.good()) {
                return false;
            }

            const std::uint32_t raw_score = static_cast<std::uint32_t>(score);

            const char bytes[4] = {
                static_cast<char>(raw_score & 0xFFu),
                static_cast<char>((raw_score >> 8) & 0xFFu),
                static_cast<char>((raw_score >> 16) & 0xFFu),
                static_cast<char>((raw_score >> 24) & 0xFFu),
            };

            stream.write(bytes, 4);
            return stream.good();
        }
    }

    return false;
}
