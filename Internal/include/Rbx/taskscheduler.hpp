#pragma once

#include <stdint.h>
#include <vector>
#include <string>
#include <optional>
#include "Rbx/instance.hpp"

namespace SB::Rbx
{
	class TaskScheduler
	{
	public:
		class Job
		{
		public:
			uintptr_t baseAddress;

			std::string getName() const;

			/// <summary>
			/// Grab stored script context from the LuaGc job.
			/// </summary>
			/// <returns></returns>
			Instance getScriptContext() const;
		};

		static TaskScheduler get();
		std::vector<Job> getJobs() const;
		std::optional<Job> getJobByName(const std::string& name) const;

		TaskScheduler(uintptr_t _baseAddress);
	
		uintptr_t baseAddress;
		uintptr_t jobsStartAddress = 0;
		uintptr_t jobsEndAddress = 0;
	};
}
