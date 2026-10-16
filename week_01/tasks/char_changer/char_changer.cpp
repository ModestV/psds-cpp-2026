#include <cctype>
#include <cstddef>

size_t CharChanger(char array[], size_t size, char delimiter) {
    size_t read = 0;
    size_t write = 0;

    while (read < size && array[read] != '\0') {
        unsigned char ch = static_cast<unsigned char>(array[read]);

        // Обработка последовательности пробельных символов.
        if (std::isspace(ch)) {
            array[write++] = delimiter;

            do {
                ++read;
            } while (read < size &&
                     array[read] != '\0' &&
                     std::isspace(
                         static_cast<unsigned char>(array[read])
                     ));

            continue;
        }

        // Преобразуем текущий символ.
        char transformed;

        if (std::isdigit(ch)) {
            transformed = '*';
        } else if (ch >= 'a' && ch <= 'z') {
            transformed = static_cast<char>(ch - 'a' + 'A');
        } else if (ch >= 'A' && ch <= 'Z') {
            transformed = static_cast<char>(ch);
        } else {
            transformed = '_';
        }

        // Подсчитываем одинаковые символы после преобразования.
        size_t count = 1;
        size_t next = read + 1;

        while (next < size && array[next] != '\0') {
            unsigned char next_ch =
                static_cast<unsigned char>(array[next]);

            if (std::isspace(next_ch)) {
                break;
            }

            char next_transformed;

            if (std::isdigit(next_ch)) {
                next_transformed = '*';
            } else if (next_ch >= 'a' && next_ch <= 'z') {
                next_transformed =
                    static_cast<char>(next_ch - 'a' + 'A');
            } else if (next_ch >= 'A' && next_ch <= 'Z') {
                next_transformed = static_cast<char>(next_ch);
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
            array[write++] = count >= 10
                ? '0'
                : static_cast<char>('0' + count);
        }

        read = next;
    }

    array[write] = '\0';
    return write;
}
