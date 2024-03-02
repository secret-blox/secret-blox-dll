#include "Rbx/rapi.hpp"
#include "Internal/memory.hpp"
#include "offsets.hpp"

#include "lobject.h"

void SB::Rbx::setup()
{
	// setup subs
	printf = (RBXPrintf*)SB_OFFSET(SB::Offsets::Rbx::Subs::printf);
	getTaskScheduler = (RBXGetTaskScheduler*)SB_OFFSET(SB::Offsets::Rbx::Subs::getTaskScheduler);
	deobfTSJobsPtr = (RBXDeobfTSJobsPtr*)SB_OFFSET(SB::Offsets::Rbx::Subs::deobfTSJobsPtr);
	// setup lua_dummynode, lua_nilobject
	rluaO_nilobject = (TValue*)SB_OFFSET(SB::Offsets::Luau::Hnilobject);
	rluaH_dummynode = (LuaNode*)SB_OFFSET(SB::Offsets::Luau::Hdummynode);
}
