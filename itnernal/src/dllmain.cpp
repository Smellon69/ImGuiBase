// All our sources etc we are linking here.
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <thread>
#include <cstdint>

#include "hooks.h"

// This is going to initialize ImGui.
void Setup(const HMODULE instance)
{
	try
	{
		gui::Setup();
		hooks::Setup();
	}
	catch (const std::exception& error)
	{
		MessageBeep(MB_ICONERROR);
		MessageBox(
			0,
			error.what(),
			"software error",
			MB_OK | MB_ICONEXCLAMATION
		);

		goto UNLOAD;
	}

	while (!GetAsyncKeyState(VK_END))
		std::this_thread::sleep_for(std::chrono::milliseconds(200));

		UNLOAD:
	hooks::Destroy();
	gui::Destroy();

	FreeLibraryAndExitThread(instance, 0);
}

// Our DLLMain entry point to load the menu.
BOOL WINAPI DLLMain(
	const HMODULE instance,
	const std::uintptr_t reason,
	const void* reserved
)
{ // Here we will check why DLLMain is being called.
	if (reason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(instance); // So this means we aren't intrested in the thread/dll calls.

		const auto thread = CreateThread( // Now we are simply creating the ImGui thread.
			nullptr,
			0,
			reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup), // Setup is the name of our function.
			instance, // Our parameter to pass our function is instance.
			0,
			nullptr
		);

		if (thread) // Last thing is to check if thread is not NULL
			CloseHandle(thread); // Here we're just closing the handle we just created.
	}

	return TRUE;
}