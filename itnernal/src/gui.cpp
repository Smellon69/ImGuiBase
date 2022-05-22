// All of the files we will be needing.

#include "gui.h"

// Our ImGui files we will need to include.

#include "../ext/imgui/imgui.h"
#include "../ext/imgui/imgui_impl_win32.h"
#include "../ext/imgui/imgui_impl_dx9.h"

#include <stdexcept> // This is so we can throw exceptions.

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND window,
	UINT message,
	WPARAM windowParam,
	LPARAM longParam
);

// Our window process
LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM windowParam,
	LPARAM longParam
);

// All of this is explained in gui.h.
bool gui::SetupWindowClass(const char* windowClassName) noexcept
{
	// Populate our WindowClass
	windowClass.cbSize = sizeof(WNDCLASSEX);
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = DefWindowProc;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hInstance = GetModuleHandle(NULL);
	windowClass.hIcon = NULL;
	windowClass.hCursor = NULL;
	windowClass.hbrBackground = NULL;
	windowClass.lpszMenuName = NULL;
	windowClass.lpszClassName = windowClassName;
	windowClass.hIconSm = NULL;

	// Now we register the class
	if (!RegisterClassEx(&windowClass))
		return false; // If it fails we return false.

	return true; // We return true if it worked.
}

void gui::DestroyWindowClass() noexcept // This will un-register our windowClass later on.
{
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept
{
	// Create a temporary window
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW, // The style of the window.
		0, // X = 0.
		0, // Y = 0.
		100, // 100 Tall.
		100, // 100 Wide.
		0, // No parent.
		0, // No menu.
		windowClass.hInstance, // The instance of the window.
		0
	);

	// Check if it was successful.
	if (!window)
		return false; // It didn't work.

	return true; // It worked.
}

void gui::DestroyWindow() noexcept
{
	if (window) // If it's valid.
		DestroyWindow(window);	// We destroy the window as the code above worked.
}

// This will be our DirectX functions.
bool gui::SetupDirectX() noexcept
{
	const auto handle = GetModuleHandle("d3d9.dll"); // We get the module handle to the DirectX .dll

	if (!handle) // If we don't have the handle the code below happens.
		return false; // Everything commits die as nothing worked.

	using CreateFn = LPDIRECT3D9(__stdcall*)(UINT); // The signature of the DirectX create function.

	const auto create = reinterpret_cast<CreateFn>(GetProcAddress( // We reinterpret Create Fn to this.
		handle,
		"Direct3dCreate9" // Name of the function.
	));

	if (!create) // If we failed to get the function.
		return false;

	// Now we actually call the function.
	d3d9 = create(D3D_SDK_VERSION);

	if (!d3d9) // If it fails.
		return false;

	// Now we make the DirectX present parameters.
	D3DPRESENT_PARAMETERS params = { };
	params.BackBufferWidth = 0;
	params.BackBufferHeight = 0;
	params.BackBufferFormat = D3DFMT_UNKNOWN;
	params.BackBufferCount = 0;
	params.MultiSampleType = D3DMULTISAMPLE_NONE;
	params.MultiSampleQuality = NULL;
	params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	params.hDeviceWindow = window;
	params.Windowed = 1;
	params.EnableAutoDepthStencil = 0;
	params.AutoDepthStencilFormat = D3DFMT_UNKNOWN;
	params.Flags = NULL;
	params.FullScreen_RefreshRateInHz = 0;
	params.PresentationInterval = 0;

	if (d3d9->CreateDevice(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_NULLREF,
		window,
		D3DCREATE_SOFTWARE_VERTEXPROCESSING | D3DCREATE_DISABLE_DRIVER_MANAGEMENT, // Set these 2 flags for the window.
		&params, // Pass the adress of our params
		&device // This is gonna be the address of the pointer where we want the device.
	) < 0) return false; // If it fails return false.

	return true; // If it sucseeds we return true
}

void gui::DestroyDirectX() noexcept
{
	if (device) // Check if device is valid
	{
		device->Release();
		device = NULL;
	}
}

// Retrieve the device in dllmain.cpp.
void gui::Setup()
{
	// Fun error time
	if (!SetupWindowClass("yeetballsquandledingle"))
		throw std::runtime_error("Failed to create window class. ");

	if (!SetupWindow("yeetballsquandledingle"))
		throw std::runtime_error("Failed to create window. ");

	if (!SetupDirectX())
		throw std::runtime_error("Failed to create device. ");

	DestroyWindow();
	DestroyWindowClass();
}

void gui::SetupMenu(LPDIRECT3DDEVICE9 device) noexcept // This will initialize ImGui.
{
	auto params = D3DDEVICE_CREATION_PARAMETERS{ };
	device->GetCreationParameters(&params);

	// Get the game window
	window = params.hFocusWindow;

	// The the games original window process
	originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess))
		);

	// Create the ImGui window context.
	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	// Initialize our ImGui
	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX9_Init(device);

	setup = true;
}

void gui::Destroy() noexcept // This will destroy everything if we want to un-inject our software
{
	// We do what we did to initialize but we use Shutdown.
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();	
	ImGui::DestroyContext();

	// We need to revert the windows process thingy so we restore WNDPROC
	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	// We just destroy DirectX
	DestroyDirectX();
}

void gui::Render() noexcept // This will just render our menu.  // void gui::Render() noexcept
{
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("amazing software", &open);
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
} 

LRESULT CALLBACK WindowProcess(
	HWND window,
	UINT message,
	WPARAM wideParam,
	LPARAM longParam
)
{
	// Here we will make our window toggle
	if (GetAsyncKeyState(VK_INSERT) & 1) // It will toggle with the INSERT key. and you can't spam the menu by holding it as we have the 1 there.
		gui::open = !gui::open; // This will toggle the opposite bool e.g if it's on it goes off etc.

	// Pass the messages to ImGui
	if (gui::open && ImGui_ImplWin32_WndProcHandler( // Only continue IF the menu is open.
		window,
		message,
		wideParam,
		longParam
	)) return 1L; // It won't call the originalWindow Process if ImGui is being clicked.

	// Here we will finally make our window process.
	return CallWindowProc( // This is so the game calls our window process and not the games.
		gui::originalWindowProcess,
		window,
		message,
		wideParam,
		longParam
	);
}