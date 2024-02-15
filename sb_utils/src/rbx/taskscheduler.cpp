#include "rbx/taskscheduler.h"
#include "rbx/api.h"
#include "offsets.h"

SB::RBX::TaskScheduler SB::RBX::TaskScheduler::get()
{
	return TaskScheduler(SB::RBX::getTaskScheduler());
}

SB::RBX::TaskScheduler::TaskScheduler(uintptr_t _baseAddress)
{
	baseAddress = _baseAddress;

	auto obfJobsPtr = *(ObfuscatedPtr2*)(baseAddress + SB::Offsets::RBX::TaskScheduler::jobs);
	SB::RBX::deobfTSJobsPtr(&obfJobsPtr);

	jobsStartAddress = obfJobsPtr.obfPtr;
	jobsEndAddress = obfJobsPtr.obfPtr2;
}
