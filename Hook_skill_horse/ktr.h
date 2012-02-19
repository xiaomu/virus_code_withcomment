#ifndef _KTR
#define _KTR
#include <windows.h>
// __declspec(dllexport)   声明一个导出函数，一般用于dll中 
#define DLL_EXPORT __declspec(dllexport)
DLL_EXPORT void SetKbHook(void);
DLL_EXPORT void RemoveKbHook(void);
#endif