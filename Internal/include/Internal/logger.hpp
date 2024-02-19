#pragma once

#include <fstream>
#include <filesystem>

namespace SB::Logger {
	extern std::ofstream logFile;
	extern std::filesystem::path dllDir;
	void setup(std::filesystem::path dllDir);
	void unload();

	void printf(const char* fmt, ...);
}