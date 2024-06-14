#pragma once
#include "includes.h"
#include <stdexcept>

namespace gui
{
	inline bool open = true;

	inline bool initialized = false;

	inline HWND window = nullptr;
	inline WNDCLASSEX windowClass = {};
	inline WNDPROC originalWindowProcess = nullptr;

	inline twglSwapBuffers oSwapBuffers = NULL;

	bool SetupWindowClass(const char* windowClassName) noexcept;
	void DestroyWindowClass() noexcept;

	bool SetupWindow(const char* windowName) noexcept;
	void DestroyWindow() noexcept;

	bool SetupHook() noexcept;

	void DestroyHook() noexcept;


	//
	void Setup();

	void SetupMenu(_In_ HDC hdc) noexcept;
	void Destroy();


	void Render() noexcept;

}