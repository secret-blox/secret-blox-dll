#pragma once

#include <array>
#include <cstdint>

#pragma warning(push)
#pragma warning(disable: 4307)
constexpr static auto timeFromString(const char* str, int offset) {
    return ((static_cast<std::uint64_t>(str[offset]) - '0') * 10) +
        (static_cast<std::uint64_t>(str[offset + 1]) - '0');
}

template<uint64_t counter>
constexpr static auto getSeedConstexpr() {
    constexpr auto t = __TIME__;
    constexpr auto nt = (timeFromString(t, 0) * 60 * 60 + timeFromString(t, 3) * 60 + timeFromString(t, 6));

    uint64_t hashedValue = 3074457345618258791ul;
    for (int i = 0; i < sizeof(uint64_t); i++) {
        hashedValue += nt & (1i64 << i) + counter;
        hashedValue *= 3074457345618258799ul;
    }

    return hashedValue;
}

constexpr uint64_t lce_a = 1273686654176231872;
constexpr uint64_t lce_c = 1278361726378617232;
constexpr uint64_t lce_m = 187236981273;

constexpr static std::uint64_t uniformDistribution(uint64_t& previous) {
    previous = ((lce_a * previous + lce_c) % lce_m);
    return previous;
}


constexpr static double uniformDistributionN(uint64_t& previous) {
    auto dst = uniformDistribution(previous);
    return static_cast<double>(dst) / lce_m;
}

template <typename T, size_t sz, uint64_t counter>
constexpr static auto uniformDistribution(T min, T max) {
    std::array<T, sz> dst{};
    auto previous = getSeedConstexpr<counter>();

    for (auto& el : dst)
        el = static_cast<T>(uniformDistributionN(previous) * (max - min) + min);

    return dst;
}
#pragma warning(pop)

#define rand_num(mi, ma) (uniformDistribution<decltype(ma), 1, __COUNTER__>(mi, ma)[0])

#define gen_rand32 (uniformDistribution<uint32_t, 1, __COUNTER__>(0, UINT32_MAX)[0])
#define gen_rand64 (uniformDistribution<uint64_t, 1, __COUNTER__>(0, UINT64_MAX)[0])