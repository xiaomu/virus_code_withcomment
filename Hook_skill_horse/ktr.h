#ifndef _KTR
#define _KTR
#include <windows.h>
// __declspec(dllexport)   ����һ������������һ������dll�� 
#define DLL_EXPORT __declspec(dllexport)
DLL_EXPORT void SetKbHook(void);
DLL_EXPORT void RemoveKbHook(void);
#endif