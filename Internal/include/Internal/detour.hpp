#include <stdint.h>


namespace SB::Detour
{
    
    extern void* freeCodecave;
    enum JumpType
    {
        Relative = 1 + sizeof(uint32_t),
        Absolute = 2 + sizeof(uint32_t) + sizeof(uintptr_t)
    };

    class Detoured
    {
        public:
            Detoured(uintptr_t srcFunc, uintptr_t dstFunc, size_t size=5);
            ~Detoured();

            bool isDetoured();
            bool destroy();
        private:
            bool _isDetoured = false;
            uintptr_t hookAddress;
            uintptr_t srcBytes;
            uintptr_t srcSize;
            JumpType originalJmp;
            JumpType hookJmp;
    };
}