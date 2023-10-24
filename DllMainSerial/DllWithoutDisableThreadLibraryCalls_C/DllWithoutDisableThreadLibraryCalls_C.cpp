// DllWithoutDisableThreadLibraryCalls_C.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <iostream>

#ifdef _MANAGED
#pragma managed(push, off)
#endif

HANDLE g_thread_handle = NULL;
HANDLE g_hEvent = NULL;

static DWORD WINAPI ThreadCreateInDllMain( LPVOID p )
{
    WaitForSingleObject( g_hEvent, INFINITE );
    return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    switch (ul_reason_for_call)   
    {
    case DLL_PROCESS_ATTACH:
        {
            g_hEvent = ::CreateEvent( NULL, FALSE, FALSE, NULL );
            g_thread_handle = ::CreateThread( NULL, 0, ThreadCreateInDllMain,NULL, 0, NULL ) ;

            // 禁止线程库调用，
            //DisableThreadLibraryCalls((HINSTANCE)hModule);
        }break;
    case DLL_PROCESS_DETACH:
        {
            ::SetEvent(g_hEvent);
            ::WaitForSingleObject(g_thread_handle, INFINITE );

            ::CloseHandle(g_thread_handle);
            g_thread_handle = NULL ;                  
            ::CloseHandle(g_hEvent);
            g_hEvent=NULL;
        } break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
        break;
    }
    return TRUE;
}
#ifdef _MANAGED
#pragma managed(pop)
#endif

