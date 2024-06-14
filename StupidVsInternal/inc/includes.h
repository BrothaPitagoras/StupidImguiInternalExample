#pragma once

/* headers imgui */
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_opengl3.h"


#include "Hook/Minhook/include/MinHook.h"


#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <wchar.h>
#include <iostream>
#include <string>
#include <TlHelp32.h>
#include <vector>
#include <format>
#include <string_view>
#include <cstdint>
#include <thread>

#pragma comment(lib,"opengl32.lib")

typedef BOOL(__stdcall* twglSwapBuffers) (_In_ HDC hDc);

#define M_PI 3.14159265358979323846
