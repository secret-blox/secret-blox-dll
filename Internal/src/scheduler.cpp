#include "Internal/scheduler.hpp"
#include "Internal/execution.hpp"
#include "Internal/logger.hpp"

#include "Rbx/taskscheduler.hpp"

uintptr_t luaGcBase = 0;
uintptr_t luaGcNewVFTable[10];
uintptr_t jobCacheFunc = 0;
uintptr_t* luaGcOldVFTable = 0;

namespace SB::Scheduler
{
    ScriptsQueue scriptsQueue;
    bool ready = false;
}

void SB::Scheduler::queueScript(std::string code)
{
    scriptsQueue.push(code);
}

uintptr_t __fastcall SB::Scheduler::cacheHook(uintptr_t job, uintptr_t unk, uintptr_t toCall)
{
    if (SB::Execution::ready && !scriptsQueue.empty())
    {
        do
        {
            auto code = scriptsQueue.front();
            scriptsQueue.pop();
            SB::Execution::execute(code);
        } while (!scriptsQueue.empty());
    }

    const auto funcOut = reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t, uintptr_t, uintptr_t)>
		(jobCacheFunc)(job, unk, toCall);

    return funcOut;
}


void SB::Scheduler::setup()
{
    if (ready)
        return;
    const auto taskScheduler = SB::Rbx::TaskScheduler::get();
    const auto luaGc = taskScheduler.getJobByName("LuaGc");
    if (!luaGc.has_value())
    {
        Logger::printf("Failed to get LuaGc job\n");
        return;
    }
    luaGcBase = luaGc->baseAddress;
    luaGcOldVFTable = *reinterpret_cast<uintptr_t**>(luaGcBase);
    memcpy(luaGcNewVFTable, luaGcOldVFTable, sizeof(luaGcNewVFTable));

    jobCacheFunc = luaGcOldVFTable[2];
    luaGcNewVFTable[2] = reinterpret_cast<uintptr_t>(cacheHook);

    *reinterpret_cast<uintptr_t**>(luaGcBase) = luaGcNewVFTable;
    ready = true;
}

void SB::Scheduler::unload()
{
    *reinterpret_cast<uintptr_t**>(luaGcBase) = luaGcOldVFTable;
    ready = false;
}