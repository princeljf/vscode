#include <stdio.h>   
#include <windows.h>
#include "ARKConsole.h"
#include "NativeAPI.h"
#include "Process.h"
#include "Module.h"
#include "Services.h"
#include "Window.h"

BOOL Help(PARM* cmd);
BOOL Exit(PARM* cmd);
void AdjustProcessTokenPrivilege();

REQUEST g_Req[] = 
{
	"QP",           EnumProcess,
	"QPM",          EnumProcMoudles,
	"QPT",          EnumProcThreads,
 	"SUPP",         SupProcess,
 	"RUMP",         RumProcess,
 	"SUPT",         SupThread,
 	"RUMT",         RumThread,
	"QPH",          EnumProcHandle,
	"QPW",          EnumProcWindow,
 	"QSRV",         EnumServices,
	"Exit",         Exit,
	"Help",         Help,
};

FnHandleCmd GetHandleFunByID(char* cmd)
{
	FnHandleCmd fun = NULL;
	int i = 0;
	for (; i < sizeof(g_Req) / sizeof(g_Req[0]); i++)
	{
		if (stricmp(cmd, g_Req[i].szCmd) == 0)
		{
			fun = g_Req[i].fun;
			break;
		}
	}
	return fun;
}

int main(void)   
{   
	// ��ʼ��δ��������
	InitNativeAPI();
	AdjustProcessTokenPrivilege();
	// ��ȡ����
	while(1)
	{
		char CmdLine[100] = {0};
		PARM parm;
		const char* tag = " "; 
		char* result = NULL;
		FnHandleCmd fun = NULL;

		// ��ȡ����
		printf(">");
		gets(CmdLine);

		// ����������ö�Ӧ����
		result = strtok(CmdLine, tag);
		if (NULL == result)
		{
			printf("error cmd \r\n");
			continue;
		}
		// ��ȡ����ָ��
		fun = GetHandleFunByID(result);

		// ��ȡ����
		result = strtok(NULL, tag);
		if (NULL != result)
		{
			parm.parm1 = atoi(result);
			result = strtok(NULL, tag);
			if (result != NULL)
			{
				parm.parm2 = atoi(result);
			}
		}

		// ���ö�Ӧ����
		if (NULL == fun)
		{
			printf("error cmd \r\n");
			continue;
		}
		if (!fun(&parm))
		{
			printf("error operation \r\n");
		}
	}
           
    system("pause");
    return 0;
}

// ������Ϣ
BOOL Help(PARM* cmd)
{
	printf("����         ��;         ����\r\n");
	printf("QP           ��ѯ������Ϣ ��  \r\n");
	printf("QPM          ��ѯ����ģ�� PID  \r\n");
	printf("QPT          ��ѯ����ģ�� PID  \r\n");
	printf("SUPP         ����ָ������ PID  \r\n");
	printf("RUMP         �ָ�ָ������ PID  \r\n");
	printf("SUPT         ����ָ���߳� TID  \r\n");
	printf("RUMT         �ָ�ָ���߳� TID  \r\n");
	printf("QPH          ��ѯ�����߳� PID  \r\n");
	printf("QPW          ��ѯ���̴��� PID  \r\n");
	printf("QSRV         ��ѯϵͳ���� PID  \r\n");
	printf("Exit         �˳�ϵͳ     ��  \r\n");	
	printf("Help         �鿴������Ϣ ��  \r\n");
	return TRUE;
}

// �˳�
BOOL Exit(PARM* cmd)
{
	exit(0);
	return TRUE;
}

//������ǰ����Ȩ��
void AdjustProcessTokenPrivilege()
{
	LUID luidTmp;
	HANDLE hToken;
	TOKEN_PRIVILEGES tkp;
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
	{
		OutputDebugString("AdjustProcessTokenPrivilege OpenProcessToken Failed ! \n");
		return;
	}
	if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &luidTmp))
	{
		OutputDebugString("AdjustProcessTokenPrivilege LookupPrivilegeValue Failed ! \n");
		CloseHandle(hToken);
		return;
	}
	tkp.PrivilegeCount = 1;
	tkp.Privileges[0].Luid = luidTmp;
	tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))
	{
		OutputDebugString("AdjustProcessTokenPrivilege AdjustTokenPrivileges Failed ! \n");
		CloseHandle(hToken);
		return;
	}
	return;
}
