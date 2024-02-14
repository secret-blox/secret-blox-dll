#include "api.h"
#include "mem.h"
#include "offsets.h"

void SB::RBX::setup()
{
	printf = (RBXPrintf*)SB_OFFSET(SB::Offsets::RBX::Subs::printf);
}
