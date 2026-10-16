#include <cctype>
#include <cstddef>


size_t CharChanger(char array[], size_t size, char delimiter) {
    size_t read = 0;
    size_t write = 0;

    while (read < size - 1) {
        char current = array[read];

        // Пробелы обрабатываем отдельно.
        // Они всегда превращаются только в один delimiter.
        if (std::isspace(static_cast<unsigned char>(current))) {
            array[write++] = delimiter;

            do {
                ++read;
            } while (read < size - 1 &&
                     std::isspace(static_cast<unsigned char>(array[read])));

            continue;
        }

        // Определяем, во что превратится текущий символ.
        if (std::isdigit(static_cast<unsigned char>(current))) {
            current = '*';
        } else if (std::islower(static_cast<unsigned char>(current))) {
            current = static_cast<char>(
                std::toupper(static_cast<unsigned char>(current))
            );
        } else if (!std::isupper(static_cast<unsigned char>(current))) {
            current = '_';
        }

        // Считаем последовательность одинаковых символов.
        size_t count = 1;
        size_t next = read + 1;

        while (next < size - 1) {
            char next_char = array[next];

            // Пробел заканчивает текущую последовательность.
            if (std::isspace(static_cast<unsigned char>(next_char))) {
                break;
            }

            if (std::isdigit(static_cast<unsigned char>(next_char))) {
                next_char = '*';
            } else if (std::islower(static_cast<unsigned char>(next_char))) {
                next_char = static_cast<char>(
                    std::toupper(static_cast<unsigned char>(next_char))
                );
            } else if (!std::isupper(static_cast<unsigned char>(next_char))) {
                next_char = '_';
            }

            if (next_char != current) {
                break;
            }

            ++count;
            ++next;
        }

        array[write++] = current;

        if (count >= 2) {
            if (count >= 10) {
                array[write++] = '0';
            } else {
                array[write++] = static_cast<char>('0' + count);
            }
        }

        read = next;
    }

    array[write] = '\0';

    return write;
}
