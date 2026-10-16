#include <cctype>
#include <cstddef>

size_t CharChanger(char array[], size_t size, char delimiter) {
    size_t read = 0;
    size_t write = 0;

    while (read < size && array[read] != '\0') {
        unsigned char original =
            static_cast<unsigned char>(array[read]);

        if (std::isspace(original)) {
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

        size_t count = 1;
        size_t next = read + 1;

        while (next < size &&
               array[next] != '\0' &&
               array[next] == array[read]) {
            ++count;
            ++next;
        }

        char transformed;

        if (std::isdigit(original)) {
            transformed = '*';
        } else if (original >= 'a' && original <= 'z') {
            transformed = static_cast<char>(
                original - 'a' + 'A'
            );
        } else if (original >= 'A' && original <= 'Z') {
            transformed = static_cast<char>(original);
        } else {
            transformed = '_';
        }

        array[write++] = transformed;

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
