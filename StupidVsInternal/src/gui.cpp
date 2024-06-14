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

	if (!gui::initialized) {
		gui::SetupMenu(hdc);
	}

	if (gui::open)
	{
		gui::Render();
	}

	return gui::oSwapBuffers(hdc);
}

void gui::Setup(){

	if (!SetupHook()) {
		throw std::runtime_error("failed to hook swapbuffers");
	}

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