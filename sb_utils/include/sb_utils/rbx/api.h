#pragma once

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

	using RBXPrintf = int(__cdecl)(MessageType, const char*, ...);
	inline RBXPrintf* printf = nullptr;
	
	void setup();
}