// DllMainSerial.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <Windows.h>
using namespace std;

static DWORD WINAPI ThreadGetModuleFileName(LPVOID h) {
    Sleep(1000);
    HMODULE hDll = (HMODULE)h;
    WCHAR wszFileName[MAX_PATH] = {0};
    GetModuleFileName( hDll, wszFileName, MAX_PATH );
    HANDLE hEvent = CreateEvent( NULL, FALSE, FALSE, L"EVENT" );
    SetEvent( hEvent );
    return 0;
}

static DWORD WINAPI ThreadGetModuleHandle(LPVOID) {
    Sleep(1000);
    GetModuleHandle( L"DllWithoutDisableThreadLibraryCalls_A.dll" );
    HANDLE hEvent = CreateEvent( NULL, FALSE, FALSE, L"EVENT" );
    SetEvent( hEvent );
    return 0;
}

static DWORD WINAPI ThreadLoadLibrary(LPVOID) {
    Sleep(1000);
    LoadLibraryW( L"DllWithoutDisableThreadLibraryCalls_A.dll" );
    HANDLE hEvent = CreateEvent( NULL, FALSE, FALSE, L"EVENT" );
    SetEvent( hEvent );
    return 0;
}

int _tmain(int argc, _TCHAR* argv[])
{
    int n = 0;
    while ( cin>>n ) {
        if ( 0 > n || 9 < n ) {
            break;
        }
        
        string strDllName;
        DWORD dwSleepTime = 0;
        switch(n) {
        case 0:{
            strDllName = "DllWithDisableThreadLibraryCalls_A";
            dwSleepTime = 100000;
               }break;
        case 1:{
            strDllName = "DllWithDisableThreadLibraryCalls_B";
            dwSleepTime = 3000;
               }break;
        case 2:{
            strDllName = "DllWithDisableThreadLibraryCalls_C";
            dwSleepTime = 3000;
               }break;
        case 3:{
            strDllName = "DllWithoutDisableThreadLibraryCalls_A";
            dwSleepTime = 3000;
               }break;
        case 4:{
            strDllName = "DllWithoutDisableThreadLibraryCalls_B";
            dwSleepTime = 3000;
               }break;
        case 5:{
            strDllName = "DllWithoutDisableThreadLibraryCalls_C";
            dwSleepTime = 3000;
               }break;
        case 6:
        case 7:
        case 8:{
            HANDLE hEvent = CreateEvent( NULL, FALSE, FALSE, L"EVENT" );
            HMODULE hKernel32 = LoadLibraryW(L"Kernel32.dll");
            if ( n == 6 ) {
                HANDLE hThread = CreateThread(NULL, 0, ThreadGetModuleFileName, hKernel32, 0, NULL);
            }
            else if ( n == 7 ) {
                HANDLE hThread = CreateThread(NULL, 0, ThreadGetModuleHandle, hKernel32, 0, NULL);
            }
            else if ( n == 8 ) {
                HANDLE hThread = CreateThread(NULL, 0, ThreadLoadLibrary, hKernel32, 0, NULL);
            }
            strDllName = "DllGetModuleHandle";
            dwSleepTime = 5000;
               }break;
        case 9:{
            strDllName = "DllLoadLibraryRingFirst";
            dwSleepTime = 5000;
               }break;
        default:
            break;
        }

        HMODULE h = LoadLibraryA(strDllName.c_str());

        if ( 8 != n ) {
            Sleep(dwSleepTime);
        }

        if ( NULL != h ) {
            FreeLibrary(h);
        }

    }
	return 0;
}

