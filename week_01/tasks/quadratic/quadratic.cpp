```cpp
#include <cmath>
#include <iomanip>
#include <iostream>
#include <utility>

void SolveQuadratic(int a, int b, int c) {
    if (a == 0) {
        if (b == 0) {
            if (c == 0) {
                std::cout << "infinite solutions";
            } else {
                std::cout << "no solutions";
            }
            return;
        }

        double x = -static_cast<double>(c) / b;

        if (x == 0.0) {
            x = 0.0;
        }

        std::cout << std::setprecision(6) << x;
        return;
    }

    const double discriminant =
        static_cast<double>(b) * b - 4.0 * a * c;

    if (discriminant < 0) {
        std::cout << "no solutions";
        return;
    }

    if (discriminant == 0) {
        double x = -static_cast<double>(b) / (2.0 * a);

        if (x == 0.0) {
            x = 0.0;
        }

        std::cout << std::setprecision(6) << x;
        return;
    }

    const double sqrt_discriminant = std::sqrt(discriminant);

    double x1 = (-b - sqrt_discriminant) / (2.0 * a);
    double x2 = (-b + sqrt_discriminant) / (2.0 * a);

    if (x1 > x2) {
        std::swap(x1, x2);
    }

    if (x1 == 0.0) {
        x1 = 0.0;
    }

    if (x2 == 0.0) {
        x2 = 0.0;
    }

    std::cout << std::setprecision(6) << x1 << " " << x2;
}
```
