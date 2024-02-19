#include "Rbx/api.hpp"
#include "Internal/memory.hpp"
#include "offsets.hpp"
#include "callstackspoof.h"

void SB::Rbx::setup()
{
	SPOOF_FUNC;
	printf = (RBXPrintf*)SB_OFFSET(SB::Offsets::Rbx::Subs::printf);
	getTaskScheduler = (RBXGetTaskScheduler*)SB_OFFSET(SB::Offsets::Rbx::Subs::getTaskScheduler);
	deobfTSJobsPtr = (RBXDeobfTSJobsPtr*)SB_OFFSET(SB::Offsets::Rbx::Subs::deobfTSJobsPtr);
}
