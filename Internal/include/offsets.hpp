#pragma once
#include <stdint.h>
#include <string>

namespace SB::Offsets
{
	namespace Rbx {
		const std::string version = "version-f2b5c592c03b4183";
		namespace Subs {
			constexpr uintptr_t printf = 0x1238f80;
			constexpr uintptr_t getTaskScheduler = 0x2919670;
			constexpr uintptr_t deobfTSJobsPtr = 0x291a1a0;
		}
		namespace Instance {
			constexpr uintptr_t descBase = 0x0;
			constexpr uintptr_t name = 0x48;
			constexpr uintptr_t children = 0x50;
			constexpr uintptr_t parent = 0x60;
		}
		namespace TaskScheduler {
			constexpr uintptr_t jobs = 0x180;
			constexpr uintptr_t jobsEnd = 0x188; // not used as it is just after jobs
		}
	}
	namespace standard {
		namespace string {

		}
	}
}