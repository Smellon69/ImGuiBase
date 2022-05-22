// Just some includes we are gonna need later on.
#pragma once
#include <d3d9.h>

// Stuff that'll maybe fix unresolved external errors.
#include "../ext/imgui/imgui_impl_dx9.h"
#include "../ext/imgui/imgui.h"

namespace gui // making gui namespace for our parameters.
{
	// Here we will set the menu to open when we inject.
	inline bool open = true;

	// When you first inject it's not set up so we make a bool set to false.
	inline bool setup = false;

	// This will be some WinAPI stuff.
	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = { };
	inline WNDPROC originalWindowProcess = nullptr; // Here we will store the original windows proccess.

	// Some DirectX Stuff.
	inline LPDIRECT3DDEVICE9 device = nullptr;
	inline LPDIRECT3D9 d3d9 = nullptr;

	// Here we will handle the set up of our windowClass function.
	bool SetupWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept; // This will un-register our windowClass later on.

	bool SetupWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;

	// This will be our DirectX functions.
	bool SetupDirectX() noexcept;
	void DestroyDirectX() noexcept;

	// Retrieve the device in dllmain.cpp.
	void Setup();

	void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept; // This will initialize ImGui.
	void Destroy() noexcept; // This will destroy everything if we want to un-inject our software

	void Render() noexcept; // This will just render our menu.
}