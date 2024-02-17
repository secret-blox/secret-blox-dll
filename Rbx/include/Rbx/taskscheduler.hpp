#pragma once

#include <stdint.h>
#include <vector>
#include <string>

namespace SB::RBX
{
	class TaskScheduler
	{
	public:
		static TaskScheduler get();
		std::vector<uintptr_t> getAllJobs() const;
		const char* getJobName(uintptr_t jobAddress) const;
		uintptr_t const getJobByName(const std::string& name) const;
		const std::intptr_t getJobContext(const std::uintptr_t jobAddress) const;

		TaskScheduler(uintptr_t _baseAddress);
	
		uintptr_t baseAddress;
		uintptr_t jobsStartAddress = 0;
		uintptr_t jobsEndAddress = 0;
	};
}
