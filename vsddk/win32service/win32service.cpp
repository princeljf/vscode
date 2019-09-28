// win32service.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <windows.h>

static TCHAR* name = TEXT("Win32Service");//��������
SERVICE_STATUS_HANDLE hStatus;//����״̬���
SERVICE_STATUS ServiceStatus;//��ǰ�����״̬��Ϣ

DWORD WINAPI HandlerEx(_In_ DWORD  dwControl, _In_ DWORD  dwEventType, _In_ LPVOID lpEventData, _In_ LPVOID lpContext)
{
	switch (dwControl)
	{
	case SERVICE_CONTROL_STOP://���ƴ��룺Ҫ��ֹֹͣͣ
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬
		return 0;
	case SERVICE_CONTROL_SHUTDOWN://���ƴ��룺�ػ�
		ServiceStatus.dwWin32ExitCode = 0;
		ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬
		return 0;
	default:
		break;
	}
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬
	return 0;
}

VOID WINAPI ServiceMain(_In_ DWORD  dwArgc, _In_ LPTSTR *lpszArgv)
{
	hStatus = RegisterServiceCtrlHandlerEx(name, &HandlerEx, NULL);
	if (hStatus == (SERVICE_STATUS_HANDLE)0)
	{
		//�����Ƕ�RegisterServiceCtrlHandlerʧ�ܵĴ���
		return;
	}

	RtlZeroMemory(&ServiceStatus, sizeof(SERVICE_STATUS));//�ṹ�����
	ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_SHUTDOWN | SERVICE_ACCEPT_STOP;//���ܹػ���ֹͣ����
	SetServiceStatus(hStatus, &ServiceStatus);//�����������״̬

	if (ServiceStatus.dwCurrentState == SERVICE_RUNNING){
		//�ѷ���Ҫ���Ĺ����ŵ�����
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
