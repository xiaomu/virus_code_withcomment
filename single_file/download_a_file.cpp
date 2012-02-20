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
        printf("下载失败！");
        break;
    case S_OK:
        printf("下载成功！");
        break;
    }
}
