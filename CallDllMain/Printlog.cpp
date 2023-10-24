#include <Windows.h>
#include <strsafe.h>
#include "Printlog.h"

void PrintLog(const char * sz, ...)
{
    char szData[512]={0};
    va_list args;
    va_start(args, sz);
    _vsnprintf_s(szData, sizeof(szData) - 1, sz, args);
    va_end(args);
    OutputDebugStringA(szData);
}
