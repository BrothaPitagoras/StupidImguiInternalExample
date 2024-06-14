#include "includes.h"

#include "gui.hpp"

void Setup(const HMODULE instance) {
    try
    {
        gui::Setup();
    }
    catch (const std::exception& error)
    {
        MessageBeep(MB_ICONERROR);
        MessageBox(
            0,
            error.what(),
            "error broder",
            MB_OK | MB_ICONEXCLAMATION
        );

        goto UNLOAD;
    }

    while (!GetAsyncKeyState(VK_HOME))
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

UNLOAD:
    gui::Destroy();

    FreeLibraryAndExitThread(instance, 0);

}


BOOL APIENTRY DllMain(HMODULE instance, uintptr_t reason, void* lpReserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(instance);

        const auto thread = 
            CreateThread(nullptr,
                0,
                reinterpret_cast<LPTHREAD_START_ROUTINE>(Setup),
                instance,
                0,
                nullptr);
        if (thread)
        {
            CloseHandle(thread);
        }
    }
    return TRUE;
}

