#include "Rbx/api.hpp"
#include "Internal/memory.hpp"
#include "offsets.hpp"

void SB::Rbx::setup()
{
	// setup subs
	printf = (RBXPrintf*)SB_OFFSET(SB::Offsets::Rbx::Subs::printf);
	getTaskScheduler = (RBXGetTaskScheduler*)SB_OFFSET(SB::Offsets::Rbx::Subs::getTaskScheduler);
	deobfTSJobsPtr = (RBXDeobfTSJobsPtr*)SB_OFFSET(SB::Offsets::Rbx::Subs::deobfTSJobsPtr);
	// TODO: setup lua_dummynode, lua_nilobject
}
