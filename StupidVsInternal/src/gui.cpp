#include "gui.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);

LRESULT CALLBACK WindowProcess(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
);

BOOL __stdcall detourSwapBuffers(_In_ HDC hdc)
{

	const auto result = gui::oSwapBuffers(hdc);

	if (!gui::initialized) {
		gui::SetupMenu(hdc);
	}

	if (gui::open)
	{
		gui::Render();
	}

	return result;
}

bool gui::SetupWindowClass(const char* windowClassName) noexcept {
	
	//populate window class

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


	//register

	if (!RegisterClassEx(&windowClass))
	{
		return false;
	}
	return true;

}

void gui::DestroyWindowClass() noexcept{
	UnregisterClass(
		windowClass.lpszClassName,
		windowClass.hInstance
	);
}

bool gui::SetupWindow(const char* windowName) noexcept{
	// create temp window
	window = CreateWindow(
		windowClass.lpszClassName,
		windowName,
		WS_OVERLAPPEDWINDOW,
		0,
		0,
		200,
		200,
		0,
		0,
		windowClass.hInstance,
		0
	);

	if (!window)
	{
		return false;
	}
	return true;
}

void gui::DestroyWindow() noexcept{
	if (window) {
		DestroyWindow(window);
	}
}
//
void gui::Setup(){
	if (!SetupWindowClass("firstTryInternalWindowClass"))
	{
		throw std::runtime_error("failed to create window class");
	}

	if (!SetupWindow("firstTryInternalWindow"))
	{
		throw std::runtime_error("failed to create window");
	}

	if (!SetupHook()) {
		throw std::runtime_error("failed to hook swapbuffers");
	}

	DestroyWindow();
	DestroyWindowClass();
}

void gui::SetupMenu(_In_ HDC hdc) noexcept
{

	window = WindowFromDC(hdc);

	originalWindowProcess = reinterpret_cast<WNDPROC>(
		SetWindowLongPtr(window, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(WindowProcess)));

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplOpenGL3_Init();

	initialized = true;

	MessageBeep(MB_ICONERROR);
	MessageBox(
		0,
		"just initialized",
		"error broder",
		MB_OK | MB_ICONEXCLAMATION
	);
}
void gui::Destroy(){
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	SetWindowLongPtr(
		window,
		GWLP_WNDPROC,
		reinterpret_cast<LONG_PTR>(originalWindowProcess)
	);

	DestroyHook();
}


void gui::Render() noexcept
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("COOL MAN",  &gui::open);
	ImGui::End();

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

}

LRESULT __stdcall WindowProcess(
	HWND hWnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam
)
{
	if (GetAsyncKeyState(VK_END) & 1)
	{
		gui::open = !gui::open;
		MessageBeep(MB_ICONERROR);
		MessageBox(
			0,
			"opened",
			"error broder",
			MB_OK | MB_ICONEXCLAMATION
		);
	}
	//pass messages to Imgui

	if (gui::open && ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) {
		return 1L;
	}


	return CallWindowProc(
		gui::originalWindowProcess,
		hWnd,
		msg,
		wParam,
		lParam
	);
}

bool gui::SetupHook() noexcept
{
	const auto handle = GetModuleHandleA("opengl32.dll");

	if (handle)
	{
		void* functionAddress = GetProcAddress(handle, "wglSwapBuffers");
		MH_Initialize();
		if (MH_CreateHook(functionAddress, detourSwapBuffers, reinterpret_cast<void**>(&oSwapBuffers))) {
			return false;
		}
		if (MH_EnableHook(functionAddress)) {
			return false;
		}
	}
	else
	{
		return false;
	}
	return true;
}
void gui::DestroyHook() noexcept
{
	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}