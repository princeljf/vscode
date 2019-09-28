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
		// 递归查找子窗口  
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);    
	}  
	return TRUE;  
}  

BOOL CALLBACK EnumWindowCallBack(HWND hWnd, LPARAM lParam)  
{  
	DWORD dwPid = 0;  
	// 获得找到窗口所属的进程  
	GetWindowThreadProcessId(hWnd, &dwPid); 
	// 判断是否为目标进程窗口
	if(dwPid == lParam) 
	{  
		CHAR szWindowName[WINDOW_TEXT_LENGTH] = {0};  
		CHAR szWindowClassName[WINDOW_TEXT_LENGTH] = {0};  
		// 输出窗口信息
		printf("0x%08X    ", hWnd); 
		//SendMessage(hWnd, WM_GETTEXT, WINDOW_TEXT_LENGTH, (LPARAM)szWindowName);
		GetWindowText(hWnd, szWindowName, WINDOW_TEXT_LENGTH);
		GetClassName(hWnd, szWindowClassName, WINDOW_TEXT_LENGTH);
		printf("%s     %s\n", szWindowName, szWindowClassName);  
		// 枚举子窗口
		EnumChildWindows(hWnd, EnumChildWindowCallBack, lParam);
	}  
	return TRUE;  
}  

// 枚举进程窗口
BOOL EnumProcWindow(PARM* Parm)
{
	DWORD ProcessId = Parm->parm1; 
	EnumWindows(EnumWindowCallBack, ProcessId);  
	return TRUE;
}
