#pragma once
#include <stdint.h>

namespace SB::RBX
{
	typedef enum {
		MESSAGE_OUTPUT,
		MESSAGE_INFO,
		MESSAGE_WARNING,
		MESSAGE_ERROR,
		MESSAGE_SENSITIVE,
		MESSAGE_TYPE_MAX
	} MessageType;

	struct ObfuscatedPtr {
		uintptr_t obfPtr;
		uintptr_t data;
		uintptr_t key;
	};
	struct ObfuscatedPtr2 {
		uintptr_t obfPtr;
		uintptr_t obfPtr2;
		uintptr_t data;
		uintptr_t data2;
		uintptr_t key;
	};

	using RBXPrintf = int __cdecl(MessageType, const char*, ...);
	inline RBXPrintf* printf = nullptr;

	using RBXGetTaskScheduler = uintptr_t __cdecl();
	inline RBXGetTaskScheduler* getTaskScheduler = nullptr;

	using RBXDeobfTSJobsPtr = uintptr_t __fastcall(void*);
	inline RBXDeobfTSJobsPtr* deobfTSJobsPtr = nullptr;
	
	void setup();
}