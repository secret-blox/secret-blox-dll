#pragma once
#include <roblox/miscellaneous/storage/storage.h>
#include <roblox/miscellaneous/structures/teb.h>

namespace module {
	enum offsets : std::intptr_t {
		/* TaskScheduler */
		schedulerJobName = 152,
		schedulerFps = 212, /* 464 */
		schedulerJobStart = 376,
		schedulerJobEnd = 384,

		/* ScriptJob */
		scriptJobDataModel = 200,
		scriptJobScriptContext = 504,

		/* datamodel */
		datamodelIsLoaded = 0,
		datamodelGameId = 2008,


		/* PropertyDescriptor */
		propertyDescriptorName = 0,
		propertyDescriptorScriptable = 0,

		/* ClassDescriptor */
		classDescriptorClassName = 0,
		classDescriptorProperties = 0,

		/* Instance */
		instanceClassDescriptor = 0,

		/* Script */
		moduleScriptBytecode = 0,
		localScriptBytecode = 0,
		scriptWeakThreadNode = 0,
		nodeWeakThreadRef = 0,
		weakLiveThreadRef = 0,
		liveThreadLuaState = 0,

		/* extraSpace */
		scriptPtr = 0,

		/* BasePart */
		partTouchPrimitive = 328,
		primitiveWorld = 456,

		/* Signals */
		signalNext = 0,
		signalFuncThread = 0,
		signalFuncRef = 0,
		signalState = 0,
		signalSlot = 0,
		signalWeakFunctionRef = 0,

		/* others */
		identityImpersonator = 392,
	};
}