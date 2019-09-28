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
	// 初始化未导出函数
	InitNativeAPI();
	AdjustProcessTokenPrivilege();
	// 获取命令
	while(1)
	{
		char CmdLine[100] = {0};
		PARM parm;
		const char* tag = " "; 
		char* result = NULL;
		FnHandleCmd fun = NULL;

		// 获取命令
		printf(">");
		gets(CmdLine);

		// 解析命令，调用对应函数
		result = strtok(CmdLine, tag);
		if (NULL == result)
		{
			printf("error cmd \r\n");
			continue;
		}
		// 获取函数指针
		fun = GetHandleFunByID(result);

		// 获取参数
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

		// 调用对应函数
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

// 帮助信息
BOOL Help(PARM* cmd)
{
	printf("命令         用途         参数\r\n");
	printf("QP           查询进程信息 无  \r\n");
	printf("QPM          查询进程模块 PID  \r\n");
	printf("QPT          查询进程模块 PID  \r\n");
	printf("SUPP         挂起指定进程 PID  \r\n");
	printf("RUMP         恢复指定进程 PID  \r\n");
	printf("SUPT         挂起指定线程 TID  \r\n");
	printf("RUMT         恢复指定线程 TID  \r\n");
	printf("QPH          查询进程线程 PID  \r\n");
	printf("QPW          查询进程窗口 PID  \r\n");
	printf("QSRV         查询系统服务 PID  \r\n");
	printf("Exit         退出系统     无  \r\n");	
	printf("Help         查看帮助信息 无  \r\n");
	return TRUE;
}

// 退出
BOOL Exit(PARM* cmd)
{
	exit(0);
	return TRUE;
}

//提升当前进程权限
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
