#include <algorithm>
#include <cmath>
#include <iomanip>
#include <ostream>
#include <stdexcept>

struct ExpTag {};
struct DegTag {};
struct AlgTag {};

class Phasor {
public:
    Phasor() = default;

    Phasor(double magnitude, double phase) {
        SetPolar(magnitude, phase);
    }

    Phasor(double magnitude, double phase, ExpTag)
        : Phasor(magnitude, phase) {}

    Phasor(double magnitude, double phase_degrees, DegTag)
        : Phasor(magnitude, phase_degrees * kPi / 180.0) {}

    Phasor(double real, double imag, AlgTag)
        : real_(real), imag_(imag) {}

    void SetPolar(double magnitude, double phase) {
        if (magnitude < 0.0) {
            magnitude = -magnitude;
            phase += kPi;
        }

        if (magnitude == 0.0) {
            real_ = 0.0;
            imag_ = 0.0;
            return;
        }

        phase = NormalizeAngle(phase);

        real_ = magnitude * std::cos(phase);
        imag_ = magnitude * std::sin(phase);
    }

    void SetCartesian(double real, double imag) {
        real_ = real;
        imag_ = imag;
    }

    double Magnitude() const {
        return std::hypot(real_, imag_);
    }

    double Phase() const {
        if (real_ == 0.0 && imag_ == 0.0) {
            return 0.0;
        }

        return NormalizeAngle(std::atan2(imag_, real_));
    }

    double PhaseDeg() const {
        return Phase() * 180.0 / kPi;
    }

    double Real() const {
        return real_;
    }

    double Imag() const {
        return imag_;
    }

    double Abs() const {
        return Magnitude();
    }

    double Angle() const {
        return Phase();
    }

    double AngleDeg() const {
        return PhaseDeg();
    }

    Phasor Conj() const {
        return Phasor(real_, -imag_, AlgTag{});
    }

    Phasor Inv() const {
        const double denominator = real_ * real_ + imag_ * imag_;

        if (denominator == 0.0) {
            throw std::domain_error("Cannot invert a zero phasor");
        }

        return Phasor(
            real_ / denominator,
            -imag_ / denominator,
            AlgTag{}
        );
    }

    Phasor& operator+=(const Phasor& other) {
        real_ += other.real_;
        imag_ += other.imag_;
        return *this;
    }

    Phasor& operator-=(const Phasor& other) {
        real_ -= other.real_;
        imag_ -= other.imag_;
        return *this;
    }

    Phasor& operator*=(const Phasor& other) {
        const double new_real =
            real_ * other.real_ - imag_ * other.imag_;

        const double new_imag =
            real_ * other.imag_ + imag_ * other.real_;

        real_ = new_real;
        imag_ = new_imag;

        return *this;
    }

    Phasor& operator/=(const Phasor& other) {
        const double denominator =
            other.real_ * other.real_ + other.imag_ * other.imag_;

        if (denominator == 0.0) {
            throw std::domain_error("Division by a zero phasor");
        }

        const double new_real =
            (real_ * other.real_ + imag_ * other.imag_) / denominator;

        const double new_imag =
            (imag_ * other.real_ - real_ * other.imag_) / denominator;

        real_ = new_real;
        imag_ = new_imag;

        return *this;
    }

    Phasor operator-() const {
        return Phasor(-real_, -imag_, AlgTag{});
    }

    friend Phasor operator+(Phasor lhs, const Phasor& rhs) {
        lhs += rhs;
        return lhs;
    }

    friend Phasor operator-(Phasor lhs, const Phasor& rhs) {
        lhs -= rhs;
        return lhs;
    }

    friend Phasor operator*(Phasor lhs, const Phasor& rhs) {
        lhs *= rhs;
        return lhs;
    }

    friend Phasor operator/(Phasor lhs, const Phasor& rhs) {
        lhs /= rhs;
        return lhs;
    }

    friend Phasor operator+(Phasor lhs, double rhs) {
        lhs.real_ += rhs;
        return lhs;
    }

    friend Phasor operator+(double lhs, Phasor rhs) {
        rhs.real_ += lhs;
        return rhs;
    }

    friend Phasor operator-(Phasor lhs, double rhs) {
        lhs.real_ -= rhs;
        return lhs;
    }

    friend Phasor operator-(double lhs, const Phasor& rhs) {
        return Phasor(lhs - rhs.real_, -rhs.imag_, AlgTag{});
    }

    friend Phasor operator*(Phasor lhs, double rhs) {
        lhs.real_ *= rhs;
        lhs.imag_ *= rhs;
        return lhs;
    }

    friend Phasor operator*(double lhs, Phasor rhs) {
        rhs.real_ *= lhs;
        rhs.imag_ *= lhs;
        return rhs;
    }

    friend Phasor operator/(Phasor lhs, double rhs) {
        if (rhs == 0.0) {
            throw std::domain_error("Division by zero scalar");
        }

        lhs.real_ /= rhs;
        lhs.imag_ /= rhs;

        return lhs;
    }

    friend Phasor operator/(double lhs, const Phasor& rhs) {
        return Phasor(lhs, 0.0, AlgTag{}) / rhs;
    }

    friend bool operator==(const Phasor& lhs, const Phasor& rhs) {
        return NearlyEqual(lhs.real_, rhs.real_) &&
               NearlyEqual(lhs.imag_, rhs.imag_);
    }

    friend bool operator!=(const Phasor& lhs, const Phasor& rhs) {
        return !(lhs == rhs);
    }

    friend std::ostream& operator<<(
        std::ostream& stream,
        const Phasor& phasor
    ) {
        const auto old_flags = stream.flags();
        const auto old_precision = stream.precision();

        stream << std::fixed << std::setprecision(3)
               << phasor.Magnitude()
               << "*e(j*" << phasor.PhaseDeg() << ") ["
               << phasor.Real()
               << " + j*" << phasor.Imag() << "]";

        stream.flags(old_flags);
        stream.precision(old_precision);

        return stream;
    }

private:
    static constexpr double kPi =
        3.141592653589793238462643383279502884;

    static constexpr double kTwoPi = 2.0 * kPi;

    static double NormalizeAngle(double angle) {
        angle = std::fmod(angle, kTwoPi);

        if (angle <= -kPi) {
            angle += kTwoPi;
        } else if (angle > kPi) {
            angle -= kTwoPi;
        }

        if (std::abs(angle + kPi) <= 1e-14) {
            return kPi;
        }

        return angle;
    }

    static bool NearlyEqual(double lhs, double rhs) {
        const double scale =
            std::max({1.0, std::abs(lhs), std::abs(rhs)});

        return std::abs(lhs - rhs) <= 1e-12 * scale;
    }

    double real_ = 0.0;
    double imag_ = 0.0;
};

inline Phasor MakePhasorCartesian(double real, double imag) {
    return Phasor(real, imag, AlgTag{});
}

inline Phasor MakePhasorPolar(double magnitude, double phase) {
    return Phasor(magnitude, phase);
}

inline Phasor MakePhasorPolarDeg(
    double magnitude,
    double phase_degrees
) {
    return Phasor(magnitude, phase_degrees, DegTag{});
}
