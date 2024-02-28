#include <random>

#include "Internal/utils.hpp"

#include <Security/xor.hpp>
#include "Security/callstackspoof.h"


std::filesystem::path SB::Utils::getDllDir(HMODULE hModule)
{
    SPOOF_FUNC;
    wchar_t Path[MAX_PATH];

    GetModuleFileNameW(hModule, Path, MAX_PATH);
    return std::filesystem::path(std::wstring(Path)).parent_path();
}
size_t SB::Utils::randomInteger(size_t min, size_t max)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(gen);
}

std::string charSet = XORSTR("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");
std::string SB::Utils::randomString(size_t length)
{
    const auto randchar = []() -> char {
        const auto maxIndex = charSet.size() - 1;
        return charSet.at(randomInteger(1, maxIndex));
    };

    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}