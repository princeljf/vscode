// ring3.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("ring3 main.\r\n");
	//打开设备
	HANDLE handle = CreateFile(L"\\\\.\\agp_link", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBoxA(0, "打开设备失败", "错误", 0);
		return 0;
	}

	getchar();
	CloseHandle(handle);
	return 0;
}
