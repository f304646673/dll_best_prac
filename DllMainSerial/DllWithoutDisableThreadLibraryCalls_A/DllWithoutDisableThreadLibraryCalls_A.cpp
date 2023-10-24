// DllWithoutDisableThreadLibraryCalls_A.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <iostream>

#ifdef _MANAGED
#pragma managed(push, off)
#endif


static DWORD WINAPI ThreadCreateInDllMain(LPVOID) {
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    DWORD tid = GetCurrentThreadId();
    switch (ul_reason_for_call)   
    {
    case DLL_PROCESS_ATTACH: {
            printf("DLL DllWithoutDisableThreadLibraryCalls_A:\tProcess attach (tid = %d)\n", tid);
            HANDLE hThread = CreateThread(NULL, 0, ThreadCreateInDllMain, NULL, 0, NULL);
            WaitForSingleObject(hThread, INFINITE);
            CloseHandle(hThread);
        }break;
    case DLL_PROCESS_DETACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

