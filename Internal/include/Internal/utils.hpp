#pragma once

#include <string>
#include <filesystem>
#include <windows.h>

namespace SB::Utils
{
    std::filesystem::path getDllDir(HMODULE hModule);

    size_t randomInteger(size_t min, size_t max);
    std::string randomString(size_t length);
}