constexpr double operator""_ft_to_m(long double value) {
    return static_cast<double>(value * 0.3048L);
}

constexpr double operator""_ft_to_cm(long double value) {
    return static_cast<double>(value * 30.48L);
}

constexpr double operator""_ft_to_in(long double value) {
    return static_cast<double>(value * 12.0L);
}


constexpr double operator""_in_to_m(long double value) {
    return static_cast<double>(value * 0.0254L);
}

constexpr double operator""_in_to_cm(long double value) {
    return static_cast<double>(value * 2.54L);
}

constexpr double operator""_in_to_ft(long double value) {
    return static_cast<double>(value / 12.0L);
}


constexpr double operator""_m_to_ft(long double value) {
    return static_cast<double>(value / 0.3048L);
}

constexpr double operator""_m_to_in(long double value) {
    return static_cast<double>(value / 0.0254L);
}

constexpr double operator""_m_to_cm(long double value) {
    return static_cast<double>(value * 100.0L);
}


constexpr double operator""_cm_to_m(long double value) {
    return static_cast<double>(value / 100.0L);
}

constexpr double operator""_cm_to_ft(long double value) {
    return static_cast<double>(value / 30.48L);
}

constexpr double operator""_cm_to_in(long double value) {
    return static_cast<double>(value / 2.54L);
}
