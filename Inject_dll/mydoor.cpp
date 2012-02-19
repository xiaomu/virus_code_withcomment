// mydoor.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <stdio.h>

// 用来创建 mydoor.dll
BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	MessageBox(NULL, "in the injected dll", "show", MB_OK);
    return TRUE;
}

