#include <cctype>
#include <cstddef>


size_t CharChanger(char array[], size_t size, char delimiter = ' ') {
    if (array == nullptr || size == 0) {
        return 0;
    }

    size_t read = 0;
    size_t write = 0;

    while (read < size - 1 && array[read] != '\0') {
        char current = array[read];

        // Преобразуем символ.
        if (std::isdigit(static_cast<unsigned char>(current))) {
            current = '*';
        } else if (std::islower(static_cast<unsigned char>(current))) {
            current = static_cast<char>(
                std::toupper(static_cast<unsigned char>(current))
            );
        } else if (current == ' ') {
            current = delimiter;
        } else if (!std::isupper(static_cast<unsigned char>(current))) {
            current = '_';
        }

        // Ищем последовательность одинаковых символов.
        size_t count = 1;
        size_t next = read + 1;

        while (next < size - 1 && array[next] != '\0') {
            char next_char = array[next];

            if (std::isdigit(static_cast<unsigned char>(next_char))) {
                next_char = '*';
            } else if (std::islower(static_cast<unsigned char>(next_char))) {
                next_char = static_cast<char>(
                    std::toupper(static_cast<unsigned char>(next_char))
                );
            } else if (next_char == ' ') {
                next_char = delimiter;
            } else if (!std::isupper(static_cast<unsigned char>(next_char))) {
                next_char = '_';
            }

            if (next_char != current) {
                break;
            }

            ++count;
            ++next;
        }

        // Пробелы после преобразования должны быть только одним delimiter.
        if (current == delimiter && delimiter == ' ') {
            array[write++] = current;
        } else {
            array[write++] = current;

            if (count > 1) {
                if (count >= 10) {
                    array[write++] = '0';
                } else {
                    array[write++] = static_cast<char>('0' + count);
                }
            }
        }

        read = next;
    }

    array[write] = '\0';

    return write;
}
