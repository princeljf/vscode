// win32service.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"

#include <windows.h>

static TCHAR* name = TEXT("Win32Service");//服务名称
SERVICE_STATUS_HANDLE hStatus;//服务状态句柄
SERVICE_STATUS ServiceStatus;//当前服务的状态信息

DWORD WINAPI HandlerEx(_In_ DWORD  dwControl, _In_ DWORD  dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP://控制代码：要求停止停止
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态
		return 0;
	case SERVICE_CONTROL_SHUTDOWN://控制代码：关机
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态
		return 0;
	default:
		break;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态
	return 0;
}

VOID WINAPI ServiceMain(_In_ DWORD  dwArgc, _In_ LPTSTR *lpszArgv)
{
	hStatus = RegisterServiceCtrlHandlerEx(name, &HandlerEx, NULL);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		//这里是对RegisterServiceCtrlHandler失败的处理
		return;
	}

	RtlZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));//结构体清空
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//接受关机和停止控制
	SetServiceStatus(hStatus, &ServiceStatus);//报告服务运行状态

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING){
		//把服务要做的工作放到这里
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[2];
	ServiceTable[0].lpServiceName = name;
	ServiceTable[0].lpServiceProc = (LPSERVICE_MAIN_FUNCTION)&ServiceMain;
	ServiceTable[1].lpServiceName = NULL;
	ServiceTable[1].lpServiceProc = NULL;
	StartServiceCtrlDispatcher(ServiceTable);
	return 0;
}
