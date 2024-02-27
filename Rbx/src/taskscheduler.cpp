#include "Rbx/taskscheduler.hpp"
#include "Rbx/rapi.hpp"
#include "offsets.hpp"

using namespace SB::Rbx;

TaskScheduler TaskScheduler::get()
{
	return TaskScheduler(getTaskScheduler());
}

TaskScheduler::TaskScheduler(uintptr_t _baseAddress)
{
	baseAddress = _baseAddress;

	auto obfJobsPtr = *(ObfuscatedPtr2*)(baseAddress + SB::Offsets::Rbx::TaskScheduler::jobs);
	SB::Rbx::deobfTSJobsPtr(&obfJobsPtr);

	jobsStartAddress = obfJobsPtr.obfPtr;
	jobsEndAddress = obfJobsPtr.obfPtr2;
}


std::vector<TaskScheduler::Job> TaskScheduler::getJobs() const {
    std::vector<TaskScheduler::Job> jobAddresses;
    size_t registeredJobs = (jobsEndAddress - jobsStartAddress) / 8;

    for (size_t i = 0; i < registeredJobs; i++) {
        uintptr_t jobAddress = *reinterpret_cast<uintptr_t*>(jobsStartAddress + i * 8);
        uintptr_t selfPtr = *reinterpret_cast<uintptr_t*>(jobAddress + SB::Offsets::Rbx::TaskScheduler::Job::self);

        if (selfPtr == jobAddress) {
            jobAddresses.push_back({ jobAddress });
        }
    }

    return jobAddresses;
}

std::optional<TaskScheduler::Job> SB::Rbx::TaskScheduler::getJobByName(const std::string& name) const
{
    const auto jobs = getJobs();

    for (const auto& job : jobs)
        if (job.getName() == name)
            return job;

    return std::nullopt;
}

std::string SB::Rbx::TaskScheduler::Job::getName() const
{
    return *reinterpret_cast<std::string*>(baseAddress + SB::Offsets::Rbx::TaskScheduler::Job::name);
}

Instance SB::Rbx::TaskScheduler::Job::getScriptContext() const
{
    return { *reinterpret_cast<uintptr_t*>(baseAddress + SB::Offsets::Rbx::TaskScheduler::Job::scriptContext) };
}
