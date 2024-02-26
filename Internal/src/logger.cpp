#include "Internal/logger.hpp"
#include "Security/security.hpp"

#include "Security/xor.hpp"
#include "Security/callstackspoof.h"

#include <cstdarg>
#include <format>
#include <chrono>

std::ofstream SB::Logger::logFile;
std::filesystem::path SB::Logger::dllDir;
void SB::Logger::setup(std::filesystem::path dllDir)
{
    SPOOF_FUNC;
    SB::Logger::dllDir = dllDir;
	logFile.open(dllDir / OBFSTR("log.txt"));
}

void SB::Logger::unload()
{
    SPOOF_FUNC;
    printf(XORSTR("Logger: Unloaded\n"));
   
	logFile.close();
}

void SB::Logger::printf(const char* fmt, ...)
{
    SPOOF_FUNC;
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
    logFile << XORSTR("[") << std::put_time(tm, XORSTR("%Y-%m-%d %H:%M:%S")) << XORSTR("] ") << result;
    logFile.flush();
}
