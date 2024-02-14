#pragma once
#include <windows.h>
#include "offsets.h"
#include <cstdint>

/*
*--------------------------------------------------------------------------------------------------------------------------------------*
															UPDATE GUIDE
*--------------------------------------------------------------------------------------------------------------------------------------*
nilObject:
go to luaopen_base (string "_G", "xpcall", "ipairs")
and go to very top and you should see something like this

```
**(int128_t**)((char*)arg1 + 8) = *(int128_t*)pseudo2addr(arg1, -10002);
*(int64_t*)((char*)arg1 + 8) = (*(int64_t*)((char*)arg1 + 8) + 0x10);
void* rax_3 = pseudo2addr(arg1, -10002);
void* var_28 = sub_202bfe4df70(arg1, "_G", 2);
```
once you found that go to pseudo2addr and boom you have your nilObject

*--------------------------------------------------------------------------------------------------------------------------------------*
dummyNode:
Search for string "size out of range" and you should be taken to `static int tcreate(lua_State* L)`
and try to find something that looks like this

```
void* rax_3 = (*(int64_t*)(arg1 + 0x10) + 0x10);
if (((rax_3 >= *(int64_t*)(arg1 + 8) || (rax_3 < *(int64_t*)(arg1 + 8) && rax_3 == &data_202c0ea7440)) || ((rax_3 < *(int64_t*)(arg1 + 8) && rax_3 != &data_202c0ea7440) && *(int32_t*)((char*)rax_3 + 0xc) <= 0)))
{
	lua_createtable(arg1, rdi, 0);
}
if (((rax_3 < *(int64_t*)(arg1 + 8) && rax_3 != &data_202c0ea7440) && *(int32_t*)((char*)rax_3 + 0xc) > 0))
{
	lua_createtable(arg1, rdi, 0);
	void* r8_1 = *(int64_t*)(arg1 + 0x10);

```

then inside lua_createtable you should see this code
```
void** rbx = *(int64_t*)(arg1 + 8);
void* rax_3 = sub_202bfe4c8f0(arg1, rbp, arg3);
*(int64_t*)rbx = rax_3;
```

decompile the sub_202bfe4c8f0 function and you should have dummynode inside of it
*--------------------------------------------------------------------------------------------------------------------------------------*
taskScheduler:
Search up string "HumanoidParallelManagerTaskQueue" and decompile the function paired to it
you should see code somewhat like this
```
int32_t rax_1 = sub_202c01e14a0(getSchedulerSingleton());
if (data_202c1e3affc < rax_1)
{
	rax_1 = data_202c1e3affc;
}
int32_t var_28 = rax_1;
int128_t var_48 = 0;
int64_t var_50 = 0x20;
int128_t var_38 = "HumanoidParallelManagerTaskQueue";
void**** var_68;
sub_202c0236430(&var_68, getSchedulerSingleton(), &var_38, &var_48, 8, &var_28);
```

as you can see it references scheduler two times in this small of a space so it shouldn't be hard to find
*--------------------------------------------------------------------------------------------------------------------------------------*
luauExecute:
Search up the string "cannot resume dead coroutine" and decompile the function paired to that
and you should see something like this towards the bottom
```
void* rax_2 = **(int64_t**)((char*)rax + 8);
if (*(int8_t*)((char*)rax_2 + 3) == 0)
{
	if (*(int8_t*)((char*)arg1 + 6) != 0)
		sub_23faff32020(arg1);

	rax = luauExecute();
}
```
luauExecute would have an arg but that's it
*--------------------------------------------------------------------------------------------------------------------------------------*
fireProximityPrompt:
to be done
*--------------------------------------------------------------------------------------------------------------------------------------*
pushInstance:
to be done
*--------------------------------------------------------------------------------------------------------------------------------------*
stdOut:
Search up string "Maximum re-entrancy depth (%i) exceeded" and decompile the function paired to it
and you should see a function like this
```
if (sub_202be849790(rbp, &arg_18, rax, r15) == 1)
{
	stdOut(3, "Maximum re-entrancy depth (%i) exceeded", ((uint64_t)data_202c1e3af04));
	sub_202bea04510();
}
```

and the one that contains the string should be robloxs stdOut/printf
*--------------------------------------------------------------------------------------------------------------------------------------*
fireClickDetector:
to be done
*--------------------------------------------------------------------------------------------------------------------------------------*
getState:
to be done
*--------------------------------------------------------------------------------------------------------------------------------------*
fireTouchInterest:
to be done
*/

#define rebase(x)  ((x - 0x26681270000) + reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)))
#define unbase(x)  (x - reinterpret_cast<uintptr_t>(GetModuleHandle(NULL)))
struct lua_State;

namespace module {
	namespace addresses {
		const uintptr_t nilObject = rebase(0x266843e7450);
		const uintptr_t dummyNode = rebase(0x36f0bf8); //updated
		const uintptr_t taskScheduler = rebase(0x2919670); //updated
		const uintptr_t luauExecute = rebase(0x26683384780);
		const uintptr_t fireProximityPrompt = rebase(0x00000000000);
		const uintptr_t pushInstance = rebase(0x00000000000);
		const uintptr_t stdOut = rebase(0x1238f80); // updated
		const uintptr_t fireClickDetector = rebase(0x000000000000);
		const uintptr_t getState = rebase(0x26681d6c690);
		const uintptr_t fireTouchInterest = rebase(0x00000000000);
	};

	namespace addressTypes {
		using taskScheduler = uintptr_t(__cdecl*)();
		using luauExecute = void(__cdecl*)(lua_State* L);
		using fireProximityPrompt = void(__cdecl*)(uintptr_t proximityPrompt);
		using pushInstance = void(__cdecl*)(lua_State* L, uintptr_t instance);
		using stdOut = void(__cdecl*)(int consoleType, const char* fmt, ...);
		using fireClickDetector = void(__cdecl*)(uintptr_t clickDetector, float distance, uintptr_t player);
		using getState = lua_State*(__cdecl*)(uintptr_t scriptContext, uintptr_t* identity, uintptr_t* script);
		using fireTouchInterest = void(__cdecl*)(uintptr_t world, uintptr_t partPrimitive1, uintptr_t partPrimitive2, int type);
	};

	namespace roblox {
		const auto taskScheduler = reinterpret_cast<addressTypes::taskScheduler>(addresses::taskScheduler);
		const auto luauExecute = reinterpret_cast<addressTypes::luauExecute>(addresses::luauExecute);
		const auto fireProximityPrompt = reinterpret_cast<addressTypes::fireProximityPrompt>(addresses::fireProximityPrompt);
		const auto pushInstance = reinterpret_cast<addressTypes::pushInstance>(addresses::pushInstance);
		const auto stdOut = reinterpret_cast<addressTypes::stdOut>(addresses::stdOut);
		const auto fireClickDetector = reinterpret_cast<addressTypes::fireClickDetector>(addresses::fireClickDetector);
		const auto getState = reinterpret_cast<addressTypes::getState>(addresses::getState);
		const auto fireTouchInterest = reinterpret_cast<addressTypes::fireTouchInterest>(addresses::fireTouchInterest);
	};
}
