// DllWithDisableThreadLibraryCalls_B.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <string>
#include <Windows.h>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HANDLE g_thread_handle = NULL;

static DWORD WINAPI ThreadCreateInDllMain( LPVOID p ) {
    Sleep(6000);
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)   
    {
    case DLL_PROCESS_ATTACH: {
        g_thread_handle = ::CreateThread(NULL,0, ThreadCreateInDllMain,(LPVOID)0, 0, NULL) ;
        // 禁止线程库调用
        DisableThreadLibraryCalls((HINSTANCE)hModule);
        }break;
    case DLL_PROCESS_DETACH: {
            // 等待内部的线程g_thread_handle 退出
            ::WaitForSingleObject(g_thread_handle, INFINITE ) ;
            // 清除资源
            ::CloseHandle(g_thread_handle);
            g_thread_handle = NULL ;
        }break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH: 
        break;
    }
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

