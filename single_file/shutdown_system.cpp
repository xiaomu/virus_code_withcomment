#include<stdio.h>
#include<windows.h>
int main(void)
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    // ȡ��ϵͳ�汾
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
            //�򿪵�ǰ���̷��ʴ���
            return false;
        LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);//��ȡ����Ψһ��ʶ�������ض�ϵͳ������Ȩ��
        tkp.PrivilegeCount = 1;
        tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
        AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
        //������������Ȩ��
    }
    ExitWindowsEx(EWX_FORCE, 0);//ǿ�ƹػ���������̷���WM_QUERYENDSESSION��Ϣ
}