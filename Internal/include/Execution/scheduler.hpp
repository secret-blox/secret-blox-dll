#include <queue>

using ScriptsQueue = std::queue<std::string>;
namespace SB::Scheduler {
    extern ScriptsQueue scriptsQueue;
    extern bool ready;

    void queueScript(std::string code);

    uintptr_t __cdecl cacheHook(uintptr_t job, uintptr_t unk, uintptr_t toCall);
    bool hookGcStep(size_t idx, uintptr_t hook);

    void setup();
    void unload();
}