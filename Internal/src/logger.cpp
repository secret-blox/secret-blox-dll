#include "Internal/logger.hpp"
#include "Internal/security.hpp"

#include <cstdarg>
#include <format>
#include <chrono>

std::ofstream SB::Logger::logFile;
void SB::Logger::setup(std::filesystem::path dllDir)
{
	logFile.open(dllDir / OBFSTR("log.txt"));
}

void SB::Logger::unload()
{
    printf("Logger: Unloaded\n");
	logFile.close();
}

void SB::Logger::printf(const char* fmt, ...)
{
    if (!logFile.is_open())
		return;
    // open args
    va_list args;
    va_start(args, fmt);
    // determine length
    va_list args_copy;
    va_copy(args_copy, args);
    int length = vsnprintf(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    // alloc mem
    std::string result;
    result.resize(length);
    vsnprintf(&result[0], length + 1, fmt, args);
    // close args
    va_end(args);
    // write to file with time prefix
    std::time_t nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    std::tm* tm = std::localtime(&nowTimeT);
    logFile << "[" << std::put_time(tm, "%Y-%m-%d %H:%M:%S") << "] " << result;
    logFile.flush();
}
