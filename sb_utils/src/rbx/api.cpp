#include "rbx/api.h"
#include "mem.h"
#include "offsets.h"

void SB::RBX::setup()
{
	printf = (RBXPrintf*)SB_OFFSET(SB::Offsets::RBX::Subs::printf);
	getTaskScheduler = (RBXGetTaskScheduler*)SB_OFFSET(SB::Offsets::RBX::Subs::getTaskScheduler);
	deobfTSJobsPtr = (RBXDeobfTSJobsPtr*)SB_OFFSET(SB::Offsets::RBX::Subs::deobfTSJobsPtr);
}
