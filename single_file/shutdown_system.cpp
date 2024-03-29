#include<stdio.h>
#include<windows.h>
int main(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // 取得系统版本
    OSVERSIONINFO osvi;
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    if (GetVersionEx(&osvi) == 0)
        return false;
    if (osvi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        // Windows NT 3.51, Windows NT 4.0, Windows 2000,
        // Windows XP,  Windows .NET Server
        if (!OpenProcessToken(GetCurrentProcess(),
                              TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
            //打开当前进程访问代号
            return false;
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);//获取本地唯一标识用于在特定系统中设置权限
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        //提升访问令牌权限
    }
    ExitWindowsEx(EWX_FORCE, 0);//强制关机，不向进程发送WM_QUERYENDSESSION消息
}