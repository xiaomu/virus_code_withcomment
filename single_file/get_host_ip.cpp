#include<stdio.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
void CheckIP(void) //����CheckIP�������������ڻ�ȡ����IP��ַ
{
    WSADATA wsaData;
    char name[255];//�������ڴ�Ż�õ��������ı���
    char *ip;//����IP��ַ����
    PHOSTENT hostinfo;
    //����MAKEWORD�������Winsock�汾����ȷֵ�����ڼ���Winsock��
    if ( WSAStartup( MAKEWORD(2, 0), &wsaData ) == 0 )
    {
        //�����Ǽ���Winsock�⣬���WSAStartup������������ֵΪ0��˵�����سɹ���������Լ���
        if( gethostname ( name, sizeof(name)) == 0)
        {
            //����ɹ��ؽ������������������name����ָ���Ļ�������
            if((hostinfo = gethostbyname(name)) != NULL)
            {
                //���ǻ�ȡ���������������������ɹ��Ļ���������һ��ָ�룬ָ��hostinfo��hostinfo
                //ΪPHOSTENT�͵ı��������漴���õ�����ṹ��
                ip = inet_ntoa (*(struct in_addr *) * hostinfo->h_addr_list);
                //����inet_ntoa������������hostinfo�ṹ�����е�h_addr_listת��Ϊ��׼�ĵ�ֱ�ʾ��IP
                //��ַ����192.168.0.1��
                printf("%s\n", ip); //���IP��ַ
            }
        }
        WSACleanup( );//ж��Winsock�⣬���ͷ�������Դ
    }
}
int main(void)
{
    CheckIP();
    return 0;
}
