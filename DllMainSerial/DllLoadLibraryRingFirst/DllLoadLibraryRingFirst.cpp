// DllLoadLibraryRingFirst.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <iostream>
using namespace std;

#ifdef _MANAGED
#pragma managed(push, off)
#endif


BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    DWORD tid = GetCurrentThreadId();
    switch (ul_reason_for_call)   
    {
    case DLL_PROCESS_ATTACH: {
        printf("DLL DllLoadLibraryRingFirst:\tProcess attach (tid = %d)\n", tid);
        LoadLibraryW(L"DllLoadLibraryRingEnd");
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

