#include "Window.h"

#define WINDOW_TEXT_LENGTH 256  

BOOL CALLBACK EnumChildWindowCallBack(HWND hWnd, LPARAM lParam)  
{  
	DWORD dwPid = 0;  
	GetWindowThreadProcessId(hWnd, &dwPid);
	if(dwPid == lParam)
	{  
		CHAR szWindowName[WINDOW_TEXT_LENGTH] = {0};  
		CHAR szWindowClassName[WINDOW_TEXT_LENGTH] = {0};  
		printf("0x%08X    ", hWnd); 
		//SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)szWindowName);  
		GetWindowText(hWnd, szWindowName, WINDOW_TEXT_LENGTH);
		GetClassName(hWnd, szWindowClassName, WINDOW_TEXT_LENGTH);
		printf("%s     %s\n", szWindowName, szWindowClassName);  
		// �ݹ�����Ӵ���  
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    
	}  
	return TRUE;  
}  

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)  
{  
	DWORD dwPid = 0;  
	// ����ҵ����������Ľ���  
	GetWindowThreadProcessId(hWnd, &dwPid); 
	// �ж��Ƿ�ΪĿ����̴���
	if(dwPid == lParam) 
	{  
		CHAR szWindowName[WINDOW_TEXT_LENGTH] = {0};  
		CHAR szWindowClassName[WINDOW_TEXT_LENGTH] = {0};  
		// ���������Ϣ
		printf("0x%08X    ", hWnd); 
		//SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)szWindowName);
		GetWindowText(hWnd, szWindowName, WINDOW_TEXT_LENGTH);
		GetClassName(hWnd, szWindowClassName, WINDOW_TEXT_LENGTH);
		printf("%s     %s\n", szWindowName, szWindowClassName);  
		// ö���Ӵ���
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}  
	return TRUE;  
}  

// ö�ٽ��̴���
BOOL EnumProcWindow(PARM* Parm)
{
	DWORD ProcessId = Parm->parm1; 
	EnumWindows(EnumWindowCallBack, ProcessId);  
	return TRUE;
}
