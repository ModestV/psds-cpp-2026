#include <cctype>
#include <cstddef>

size_t CharChanger(char array[], size_t size, char delimiter) {
    if (size == 0) {
        return 0;
    }

    size_t read = 0;
    size_t write = 0;

    while (read < size - 1 && array[read] != '\0') {
        unsigned char current =
            static_cast<unsigned char>(array[read]);

        // Последовательность пробельных символов.
        if (std::isspace(current)) {
            array[write++] = delimiter;

            do {
                ++read;
            } while (read < size - 1 &&
                     array[read] != '\0' &&
                     std::isspace(
                         static_cast<unsigned char>(array[read])
                     ));

            continue;
        }

        // Преобразуем текущий символ.
        char transformed;

        if (std::isdigit(current)) {
            transformed = '*';
        } else if (current >= 'a' && current <= 'z') {
            transformed = static_cast<char>(current - 'a' + 'A');
        } else if (current >= 'A' && current <= 'Z') {
            transformed = static_cast<char>(current);
        } else {
            transformed = '_';
        }

        // Считаем последовательность одинаковых символов
        // после преобразования.
        size_t count = 1;
        size_t next = read + 1;

        while (next < size - 1 && array[next] != '\0') {
            unsigned char ch =
                static_cast<unsigned char>(array[next]);

            if (std::isspace(ch)) {
                break;
            }

            char next_transformed;

            if (std::isdigit(ch)) {
                next_transformed = '*';
            } else if (ch >= 'a' && ch <= 'z') {
                next_transformed = static_cast<char>(ch - 'a' + 'A');
            } else if (ch >= 'A' && ch <= 'Z') {
                next_transformed = static_cast<char>(ch);
            } else {
                next_transformed = '_';
            }

            if (next_transformed != transformed) {
                break;
            }

            ++count;
            ++next;
        }

        array[write++] = transformed;

        if (count >= 2) {
            array[write++] = (count >= 10)
                ? '0'
                : static_cast<char>('0' + count);
        }

        read = next;
    }

    array[write] = '\0';

    return write;
}
