#include <windows.h>
#include <iostream.h>
#include <tlhelp32.h>
#include <stdio.h>

// ˼·: ��Ŀ������д���һ���µ��߳�, ������̴߳���Ҫע���Dll�ĵ�ַ


BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId);
int EnableDebugPriv(const char *name);
DWORD GetProcessId();

int main()
{
	char myFile[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, myFile);
	strcat(myFile, "\\mydoor.dll");
	InjectDll(myFile, GetProcessId());

	return 0;
}

BOOL InjectDll(const char *DllFullPath, const DWORD dwRemoteProcessId)
{
	HANDLE hRemoteProcess;
	
	// ����Ȩ��, ���Դ���������
	EnableDebugPriv(SE_DEBUG_NAME);
	
	// OpenProcess(): Opens an existing local process object.
	hRemoteProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwRemoteProcessId);
	
	//VirtualAllocEx(): Reserves or commits a region of memory within the virtual address space of a specified process. 
	//							The function initializes the memory it allocates to zero, unless MEM_RESET is used.
	char *pszLibFileRemote = NULL;
	pszLibFileRemote = (char *)VirtualAllocEx(hRemoteProcess, NULL, lstrlen(DllFullPath)+1, MEM_COMMIT, PAGE_READWRITE);
	
	//WriteProcessMemory(): Writes data to an area of memory in a specified process. 
	//									The entire area to be written to must be accessible or the operation fails.
	// ��DllFullPath������д����һ�򿪵Ľ�����
	WriteProcessMemory(hRemoteProcess, pszLibFileRemote, (void *)DllFullPath, lstrlen(DllFullPath)+1, NULL);
	
	PTHREAD_START_ROUTINE pfnStartAddr = (PTHREAD_START_ROUTINE) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
	
	//CreateRemoteThread(): Creates a thread that runs in the virtual address space of another process.
	// pfnStartAddr ָ��ĺ��������pszLibFileRemote
	HANDLE hRemoteThread;
	if((hRemoteThread = CreateRemoteThread(hRemoteProcess, NULL, 0,  pfnStartAddr, pszLibFileRemote, 0, NULL)) == NULL)
	{
		cout<< "ע���߳�ʧ��!" << endl;
		return false;
	}
	
	CloseHandle(hRemoteProcess);
	CloseHandle(hRemoteThread);
	
	return true;
}
	
int EnableDebugPriv(const char *name)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	
	//OpenProcessToken(): opens the access token associated with a process
	OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY, &hToken);
	
	//LookupPrivilegeValue:()retrieves the locally unique identifier (LUID) used on a specified system to locally represent the specified privilege name.
	LookupPrivilegeValue(NULL, name, &luid);
	
	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	
	AdjustTokenPrivileges(hToken, 0, &tp, sizeof(TOKEN_PRIVILEGES), NULL, NULL);
	
	return 0;
}
	
DWORD GetProcessId()
{
	DWORD Pid = -1;
	
	//CreateToolHelp32Snapshot(): Takes a snapshot of the specified processes, as well as the heaps, modules, and threads used by these processes.
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	
	PROCESSENTRY32 lPrs;
	ZeroMemory(&lPrs, sizeof(lPrs));
	lPrs.dwSize = sizeof(lPrs);
	char *targetFile = "calc.exe";
	Process32First(hSnap, &lPrs);
	if(strstr(targetFile, lPrs.szExeFile))
	{
		Pid = lPrs.th32ProcessID;
		return Pid;
	}
	
	while(1)
	{
		ZeroMemory(&lPrs, sizeof(lPrs));
		lPrs.dwSize = sizeof(lPrs);
		if(!Process32Next(hSnap, &lPrs))
		{
			Pid = -1;
			break;
		}
		if(strstr(targetFile, lPrs.szExeFile))
		{
			Pid = lPrs.th32ProcessID;
			break;
		}
	}
	
	return Pid;
}