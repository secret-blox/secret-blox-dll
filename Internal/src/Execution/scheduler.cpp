#include "Execution/scheduler.hpp"
#include "Execution/execution.hpp"
#include "Internal/logger.hpp"

#include "Rbx/taskscheduler.hpp"

#include "Security/xor.hpp"

uintptr_t baseAddress = 0;
uintptr_t newVFTable[10];
uintptr_t jobCacheFunc = 0;
uintptr_t* oldVFTable = 0;

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
    const auto job = taskScheduler.getJobByName("WaitingHybridScriptsJob");
    if (!job.has_value())
    {
        Logger::printf("Failed to get LuaGc job\n");
        return;
    }
    SB::Logger::printf(XORSTR("Copying Job VFTable\n"));
    baseAddress = job->baseAddress;
    oldVFTable = *reinterpret_cast<uintptr_t**>(baseAddress);
    memcpy(newVFTable, oldVFTable, sizeof(newVFTable));

    jobCacheFunc = oldVFTable[2];
    newVFTable[2] = reinterpret_cast<uintptr_t>(cacheHook);

    SB::Logger::printf(XORSTR("Hooking Job VFTable\n"));
    *reinterpret_cast<uintptr_t**>(baseAddress) = newVFTable;
    ready = true;
}

void SB::Scheduler::unload()
{
    *reinterpret_cast<uintptr_t**>(baseAddress) = oldVFTable;
    ready = false;
}