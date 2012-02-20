//#include "stdafx.h"
//#include "GetSysInfo.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
CWinApp theApp;
using namespace std;
// ��ȡCPU�����Ϣ
void GetCPUInfo()
{
    CString CPU;
    SYSTEM_INFO systeminfo;
    // ��ȡCPU�ͺ�
    GetSystemInfo (&systeminfo);
    switch (systeminfo.dwProcessorType)
    {
    case PROCESSOR_INTEL_386: // 386ϵ��
        CPU = "PROCESSOR_INTEL_386";
        break;
    case PROCESSOR_INTEL_486: // 486ϵ��
        CPU = "PROCESSOR_INTEL_486";
        break;
    case PROCESSOR_INTEL_PENTIUM: // ����ϵ��
        CPU = "PROCESSOR_INTEL_PENTIUM";
        break;
    case PROCESSOR_MIPS_R4000:
        CPU = "PROCESSOR_MIPS_R4000";
        break;
    case PROCESSOR_HITACHI_SH3:
        CPU = "PROCESSOR_HITACHI_SH3";
        break;
    case PROCESSOR_HITACHI_SH4:
        CPU = "PROCESSOR_HITACHI_SH4";
        break;
    case PROCESSOR_STRONGARM:
        CPU  = "PROCESSOR_STRONGARM";
        break;
    case PROCESSOR_ARM720:
        CPU = "PROCESSOR_ARM720";
        break;
    default:
        CPU = "Unknown Type";
        break;
    }
    printf("\n\nCPU �ͺţ�%s\n\n", CPU);

}
// ��ȡ�ڴ���Ϣ
void GetMemoryInfo()
{
    CString MemoryTotal; // ���ڴ��С
    CString MemoryAvail; // �����ڴ��С


    MEMORYSTATUS memory;
    memory.dwLength = sizeof(memory); //��ʼ��
    // ��ȡ�ڴ���Ϣ
    GlobalMemoryStatus(&memory);
    MemoryTotal.Format(_T("%d Mb"), int(memory.dwTotalPhys / 1024 / 1024));
    MemoryAvail.Format(_T("%d Mb"), int(memory.dwAvailPhys / 1024 / 1024));
    printf("�������ڴ棺%s\n\n", MemoryTotal);
    printf("���������ڴ棺%s\n\n", MemoryAvail);
}

// ��ȡ������Ϣ
void GetDiskInfo()
{
    DWORD sector, byte, cluster, free;
    long int freespace, totalspace;
    UINT type;
    char name;
    CString strname;
    // ѭ�����A��Z, �ҵ����̼����
    //0��δ֪�̡�1�������ڡ�2�����ƶ����̡�3���̶�����
    //4��������̡�5��CD��ROM��6���ڴ�������
    for (name = 'A'; name <= 'Z'; name++)
    {
        strname.Format(_T("%c:"), name);
        type = GetDriveType(strname); //��ô�������
        switch(type)
        {
        case 0:
            printf("δ֪���ʹ���: %s", strname);
            break;
        case 2:
            printf("���ƶ����ʹ���: %s", strname);
            break;
        case 3:
            printf("�̶�����: %s", strname);
            break;
        case 4:
            printf("����ӳ�����: %s", strname);
            break;
        case 5:
            printf("����: %s", strname);
            break;
        case 6:
            printf("�ڴ��������: %s", strname);
            break;
        default:
            break;
        }
        if(type != 1)
        {
            // ��ȡ���̿ռ���Ϣ
            GetDiskFreeSpace(strname, &sector, &byte, &free, &cluster);
            // �����ܴ�С
            totalspace = int(cluster) * byte * sector / 1024 / 1024;
            // ���̿��ÿռ��С
            freespace = int(free) * byte * sector / 1024 / 1024;
            printf(" �ܴ�С %d Mb", totalspace);
            printf(" ���ÿռ� %d Mb", freespace);
            printf("\n");
        }
    }

}
int _t main(int argc, TCHAR *argv[], TCHAR *envp[])
{
    int nRetCode = 0;

    // initialize MFC and print and error on failure
    if (!AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0))
    {
        // TODO: change error code to suit your needs
        cerr << _T("Fatal Error: MFC initialization failed") << endl;
        nRetCode = 1;
    }
    else
    {
        // TODO: code your application's behavior here.
        // ���CPU��Ϣ
        GetCPUInfo();
        // ����ڴ���Ϣ
        GetMemoryInfo();
        // ���������Ϣ
        GetDiskInfo();
    }

    return nRetCode;
}
