// 单线程TCP扫描器
// 扫描哪一些端口可以连接

#include<winsock2.h>
#include<stdio.h>
#include <time.h>//计时需要用到的头文件
#pragma comment(lib,"ws2_32.lib")

#include <time.h>//计时需要用到的头文件
clock_t start, end; //程序运行的起始和结束时间
float costtime;//程序耗时
void usage(void)
{
    printf("\tusage: tcpscan RemoteIP StartPort-EndPort\n");
    printf("\tExample: tcpscan 192.168.1.1 80-100\n");
}
int main(int argc, char **argv)
{
    char *host;
    int startport, endport;
    char *p;
    if(argc != 3)
    {
        usage();
        return 0;
    }

    p = argv[2]; //处理端口参数
    if(strstr(argv[2], "-"))
    {
        startport = atoi(argv[2]);
        for(; *p;)
            if(*(p++) == '-')break;
        endport = atoi(p);

        if(startport < 1 || endport > 65535)
        {
            printf("Port Error!\n");
            return 0;
        }
    }
    host = argv[1];
    WSADATA ws;
    SOCKET s;
    struct sockaddr_in addr;
    int result;
    long lresult;
    lresult = WSAStartup(MAKEWORD(1, 1), &ws);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr(host);
    start = clock(); //开始计时
    for (int i = startport; i < endport; i++)
    {
        s = socket(AF_INET, SOCK_STREAM, 0);
        addr.sin_port = htons(i);
        if(s == INVALID_SOCKET)break;
        result = connect(s, (struct sockaddr *)&addr, sizeof(addr));
        if(result == 0)
        {
            printf("%s %d\n", host, i);
            closesocket(s);
        }
    }
    end = clock(); //计时结束
    costtime = (float)(end - start) / CLOCKS_PER_SEC; //转换时间格式
    printf("Cost time:%f second", costtime); //显示耗时
    WSACleanup();
}
