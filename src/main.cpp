#pragma comment(lib, "user32")

#include <thread>

#include <Windows.h>

void thread(HMODULE module) {
	Beep(750, 300);

	FreeLibraryAndExitThread(module, 0);
}

BOOL WINAPI DllMain(HMODULE module, DWORD reason, LPVOID) {
	DisableThreadLibraryCalls(module);

	if (reason == DLL_PROCESS_ATTACH) {
		std::thread(thread, module).detach();
	}

	return TRUE;
}

extern "C" __declspec(dllexport) int GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) {
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}
