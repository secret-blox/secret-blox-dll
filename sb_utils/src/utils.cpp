#include "utils.h"

std::filesystem::path SB::Utils::getDllDir(HMODULE hModule)
{
    wchar_t Path[MAX_PATH];
    GetModuleFileNameW(hModule, Path, MAX_PATH);
    return std::filesystem::path(std::wstring(Path)).parent_path();
}