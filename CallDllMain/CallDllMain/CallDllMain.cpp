// CallDllMain.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <string>
#include "../Printlog.h"

std::string g_strA = "A";
std::string g_strB = "B";
std::string g_strC = "C";
std::string g_strD = "D";

#define WAIT() \
    Sleep(1000);\
    PrintLog("\n");\

static DWORD WINAPI ThreadRoutine(LPVOID lpParam) {
    DWORD dwTID = GetCurrentThreadId();
    PrintLog("Thread%s %u\n", (LPSTR)lpParam, dwTID );
    Sleep(15000);
    PrintLog("\nThread%s Will Exit\n", (LPSTR)lpParam );
    return 0;
}

static DWORD WINAPI ThreadFun(LPVOID lpParam){
    DWORD dwMainTID = GetCurrentThreadId();
    PrintLog("MainTid:%u\n", dwMainTID);
    PrintLog("\n");

    PrintLog("CreatThread A\n");
    CreateThread( NULL, NULL, ThreadRoutine, (LPVOID)g_strA.c_str(), 0, NULL );
    WAIT();

    PrintLog("LoadLibraryA Dll1\n");
    HMODULE hDll1 = LoadLibraryA("DLL1");
    WAIT();

    PrintLog("CreatThread B\n");
    CreateThread( NULL, NULL, ThreadRoutine, (LPVOID)g_strB.c_str(), 0, NULL );
    WAIT();

    PrintLog("LoadLibraryA Dll2\n");
    HMODULE hDll2 = LoadLibraryA("DLL2");
    WAIT();

    PrintLog("CreatThread C\n");
    CreateThread( NULL, NULL, ThreadRoutine, (LPVOID)g_strC.c_str(), 0, NULL );
    WAIT();

    PrintLog("CreatThread D\n");
    HANDLE hThreadD = CreateThread( NULL, NULL, ThreadRoutine, (LPVOID)g_strD.c_str(), 0, NULL );
    WAIT();

    //////////////////////////////////////////////////////////////////////////
#ifdef FREEDLL1
    FreeLibrary(hDll1);
    WAIT();
#endif

#ifdef FREEDLL2
    FreeLibrary(hDll2);
    WAIT();
#endif
    //////////////////////////////////////////////////////////////////////////

    PrintLog("TerminateThread D\n");
    TerminateThread(hThreadD, 0);
    WAIT();

    Sleep(25000);

#ifdef TP
    TerminateProcess(GetCurrentProcess(),-1);
#endif // TP

#ifdef EP
    ExitProcess(-1);
#endif // EP

#ifdef TT
    TerminateThread(GetCurrentThread(),-1);
#endif // TT

#ifdef ET
    ExitThread(-1);
#endif // ET
    return 0;
}

#define RUNINTHREAD

int _tmain(int argc, _TCHAR* argv[])
{
#ifdef MULTILOADFREE
    DWORD dwMainTID = GetCurrentThreadId();
    PrintLog("MainThreadTid :%u\n", dwMainTID);
    PrintLog("\n");

    PrintLog("LoadLibraryA1\n");
    HMODULE hDll1 = LoadLibraryA("DLL1");
    WAIT();

    PrintLog("LoadLibraryA2\n");
    hDll1 = LoadLibraryA("DLL1");
    WAIT();

    PrintLog("LoadLibraryA3\n");
    hDll1 = LoadLibraryA("DLL1");
    WAIT();
#endif // MULTILOADFREE
    

#ifdef RUNINTHREAD
    CreateThread( NULL, NULL, ThreadFun, NULL, 0, NULL );
    Sleep(35000);
#else
    ThreadFun(NULL);
#endif // RUNINTHREAD

#ifdef MULTILOADFREE
    PrintLog("FreeLibrary1\n");
    FreeLibrary(hDll1);
    WAIT();

    PrintLog("FreeLibrary2\n");
    FreeLibrary(hDll1);
    WAIT();

    PrintLog("FreeLibrary3\n");
    FreeLibrary(hDll1);
    WAIT();

    hDll1 = LoadLibraryA("DLL1");
#endif // MULTILOADFREE

    PrintLog("\nProceess Exit\n");
	return 0;
}

