// Dll2.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <string>
#include "../Printlog.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif
using namespace std;

BOOL APIENTRY DllMain( HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                      )
{
    string strReason;
    DWORD TID = GetCurrentThreadId();
    switch (ul_reason_for_call)   
    {
    case DLL_PROCESS_ATTACH:{
        strReason = "DLL_PROCESS_ATTACH";
                            }break;
    case DLL_PROCESS_DETACH:{
        strReason = "DLL_PROCESS_DETACH";
                            }break;
    case DLL_THREAD_ATTACH:{
        strReason = "DLL_THREAD_ATTACH";
                           }break;
    case DLL_THREAD_DETACH:{
        strReason = "DLL_THREAD_DETACH";
                           }break;
    default:{
        strReason = "default";
            }break;
    }
    PrintLog("Dll2 TID:%u %s\n", TID, strReason.c_str() );
    return TRUE;
}
#ifdef _MANAGED
#pragma managed(pop)
#endif

