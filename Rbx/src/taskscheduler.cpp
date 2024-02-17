#include "Rbx/taskscheduler.hpp"
#include "Rbx/api.hpp"
#include "offsets.hpp"

SB::RBX::TaskScheduler SB::RBX::TaskScheduler::get()
{
	return TaskScheduler(SB::RBX::getTaskScheduler());
}

SB::RBX::TaskScheduler::TaskScheduler(uintptr_t _baseAddress)
{
	baseAddress = _baseAddress;

	auto obfJobsPtr = *(ObfuscatedPtr2*)(baseAddress + SB::Offsets::Rbx::TaskScheduler::jobs);
	SB::RBX::deobfTSJobsPtr(&obfJobsPtr);

	jobsStartAddress = obfJobsPtr.obfPtr;
	jobsEndAddress = obfJobsPtr.obfPtr2;
}


std::vector<uintptr_t> SB::RBX::TaskScheduler::getAllJobs() const {
    std::vector<uintptr_t> jobAddresses;
    int registeredJobs = (jobsEndAddress - jobsStartAddress) / 8;

    for (size_t i = 0; i < registeredJobs; i++) {
        uintptr_t jobAddress = *reinterpret_cast<uintptr_t*>(jobsStartAddress + i * 8);
        uintptr_t selfPtr = *reinterpret_cast<uintptr_t*>(jobAddress + 0x8);

        if (selfPtr == jobAddress) {
            jobAddresses.push_back(jobAddress);
        }
    }

    return jobAddresses;
}

const char* SB::RBX::TaskScheduler::getJobName(uintptr_t jobAddress) const {
    return reinterpret_cast<const char*>(jobAddress + 152);
}

const uintptr_t SB::RBX::TaskScheduler::getJobByName(const std::string& name) const {
    std::vector<uintptr_t> jobs = getAllJobs();

    for (uintptr_t jobAddress : jobs) {
        const char* jobName = getJobName(jobAddress);
        if (jobName && name == jobName) {
            return jobAddress;
        }
    }

    return 0; 
}

const std::intptr_t SB::RBX::TaskScheduler::getJobContext(const std::uintptr_t job) const {
    // TODO: Implement get job context
    return 0;
}