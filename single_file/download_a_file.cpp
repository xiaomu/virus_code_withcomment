#include <windows.h>
#include <stdio.h>
#pragma comment(lib,"Urlmon")
void main()
{
    HRESULT urldown;
    urldown = URLDownloadToFile(NULL, "http://12.1.40.217/manual/test.txt", "c:\\test.txt", 0, NULL);
    switch(urldown)
    {
    case E_OUTOFMEMORY:
        printf("����ʧ�ܣ�");
        break;
    case S_OK:
        printf("���سɹ���");
        break;
    }
}
