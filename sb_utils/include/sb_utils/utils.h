#pragma once

#include <filesystem>
#include <windows.h>

namespace SB::Utils
{
    std::filesystem::path getDllDir(HMODULE hModule);
}