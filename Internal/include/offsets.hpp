#pragma once
#include <stdint.h>
#include <string>

/*
* This file contains offsets necessary for the api functionality.
* Prior to updating SB::Offsets::Rbx::version, ensure all relevant offsets are updated.
* [IMPORTANT] Offset is primarily utilized within our API and requires updates.
* [RAREUPDATE] Offset is unlikely to change frequently.
* [DEPRECATED] Offset is no longer utilized within our API.
*/


namespace SB::Offsets
{
	namespace Rbx {
		const std::string version = "version-85c66b72a4fe4a74";
		namespace Subs {
			constexpr uintptr_t printf = 0x122bc10;				// [IMPORTANT]
			constexpr uintptr_t getTaskScheduler = 0x290e2d0;	// [IMPORTANT]
			constexpr uintptr_t deobfTSJobsPtr = 0x290ee00;		// [IMPORTANT]
		}
		namespace Instance {
			constexpr uintptr_t descBase = 0x0;		// [RAREUPDATE]
			constexpr uintptr_t name = 0x48;		// [RAREUPDATE] [IMPORTANT]
			constexpr uintptr_t children = 0x50;	// [RAREUPDATE] [IMPORTANT]
			constexpr uintptr_t parent = 0x60;		// [RAREUPDATE] [IMPORTANT]
		}
		namespace TaskScheduler {
			constexpr uintptr_t jobs = 0x180;	// [RAREUPDATE] [IMPORTANT]
			constexpr uintptr_t jobsEnd = 0x188; // [RAREUPDATE] [DEPRECATED] (just after jobs)
		}
	}
	namespace standard {
		namespace string {

		}
	}
}