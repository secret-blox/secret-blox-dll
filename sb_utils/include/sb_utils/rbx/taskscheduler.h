#pragma once

#include <stdint.h>

namespace SB::RBX
{
	class TaskScheduler
	{
	public:
		static TaskScheduler get();
		TaskScheduler(uintptr_t _baseAddress);
	
		uintptr_t baseAddress;
		uintptr_t jobsStartAddress = 0;
		uintptr_t jobsEndAddress = 0;
	};
}