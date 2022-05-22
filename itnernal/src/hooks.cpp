#include "hooks.h"

#include <stdexcept>
#include <intrin.h>

#include "../ext/minhook/minhook.h"

#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"

void hooks::Setup()
{
	// If minhook don't work we give them a error.
	if (MH_Initialize())
		throw std::runtime_error("Unable to initialize minhook");

	if (MH_CreateHook(
		VirtualFunction(gui::device, 16),
		&Reset,
		reinterpret_cast<void**>(&ResetOriginal)
	)) throw std::runtime_error("Unable to hook EndScene()");

	if (MH_CreateHook(
		VirtualFunction(gui::device, 42),
		&EndScene,
		reinterpret_cast<void**>(&EndSceneOriginal)
	)) throw std::runtime_error("Unable to hook EndScene()");


	if (MH_EnableHook(MH_ALL_HOOKS))
		throw std::runtime_error("Unable to enable hooks");

	gui::DestroyDirectX();
}

void hooks::Destroy() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

long __stdcall hooks::EndScene(IDirect3DDevice9* device) noexcept
{
	static const auto returnAddress = _ReturnAddress();

	const auto result = EndSceneOriginal(device, device);

	// Stop endscene from getting called twice
	if (_ReturnAddress() == returnAddress)
		return result;

	if (!gui::setup)
		gui::SetupMenu(device);

	return result;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* params) noexcept
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
	const auto result = ResetOriginal(device, device, params);
	ImGui_ImplDX9_CreateDeviceObjects();
	return result;
}
