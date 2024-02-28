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
	#define OFFSET constexpr uintptr_t
	namespace Luau {
		OFFSET Hdummynode = 0x36774e8;				// [IMPORTANT]
		OFFSET Hnilobject = 0x3677780;				// [IMPORTANT]
		namespace Userdata {
			OFFSET identity = 0x30;					// [RAREUPDATE] [IMPORTANT]
			OFFSET capFlags = 0x40;					// [RAREUPDATE] [IMPORTANT]
			OFFSET capFlags2 = 0x48;				// [RAREUPDATE] [IMPORTANT]
		}
	};
	namespace Rbx {
		const std::string version = "version-70a2467227df4077";
		namespace Subs {
			OFFSET printf = 0x11c3170;				// [IMPORTANT?]
			OFFSET getTaskScheduler = 0x28a1900;	// [IMPORTANT]
			OFFSET deobfTSJobsPtr = 0x28a2430;		// [IMPORTANT]
			OFFSET pseudo2addr = 0x233d2c0;			// [IMPORTANT]
		}
		namespace Reflection {
			namespace Descriptor {
				OFFSET name = 0x8;	    // [RAREUPDATE] [IMPORTANT] (RBX::Name)
			}
		}
		namespace Instance {
			OFFSET self = 0x8;			// [RAREUPDATE] [IMPORTANT]
			OFFSET descriptor = 0x18;	// [RAREUPDATE] [IMPORTANT] (RBX::Reflection::ClassDescriptor*)
			OFFSET name = 0x48;			// [RAREUPDATE] [IMPORTANT]
			OFFSET children = 0x50;		// [RAREUPDATE] [IMPORTANT]
			OFFSET parent = 0x60;		// [RAREUPDATE] [IMPORTANT]
		}
		namespace TaskScheduler {
			OFFSET jobs = 0x180;		// [RAREUPDATE] [IMPORTANT]
			OFFSET jobsEnd = 0x188;		// [RAREUPDATE] [DEPRECATED] (just after jobs)
			namespace Job {
				OFFSET self = 0x8;		// [RAREUPDATE] [IMPORTANT]
				OFFSET name = 0x98;		// [RAREUPDATE] [IMPORTANT]
				OFFSET scriptContext = 0x1f0; // [RAREUPDATE] [IMPORTANT]
			}
		}
	}
	namespace standard {
		namespace string {

		}
	}
}