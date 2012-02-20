// ���߳�TCPɨ����
// ɨ����һЩ�˿ڿ�������

#include<winsock2.h>
#include<stdio.h>
#include <time.h>//��ʱ��Ҫ�õ���ͷ�ļ�
#pragma comment(lib,"ws2_32.lib")

#include <time.h>//��ʱ��Ҫ�õ���ͷ�ļ�
clock_t start, end; //�������е���ʼ�ͽ���ʱ��
float costtime;//�����ʱ
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

    p = argv[2]; //����˿ڲ���
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
    start = clock(); //��ʼ��ʱ
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
    end = clock(); //��ʱ����
    costtime = (float)(end - start) / CLOCKS_PER_SEC; //ת��ʱ���ʽ
    printf("Cost time:%f second", costtime); //��ʾ��ʱ
    WSACleanup();
}
