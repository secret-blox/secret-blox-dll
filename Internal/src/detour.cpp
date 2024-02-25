#include "Internal/detour.hpp"

extern "C" {
    extern unsigned char srcCodecave[];
}
void* SB::Detour::freeCodecave = &srcCodecave;
size_t wroteBytes = 0;

bool SB::Detour::Detoured::isDetoured()
{
    return _isDetoured;
}

SB::Detour::Detoured::Detoured(uintptr_t srcFunc, uintptr_t dstFunc, size_t size)
{
    
}

bool SB::Detour::Detoured::destroy()
{
    return false;
}

SB::Detour::Detoured::~Detoured()
{
    destroy();
}

