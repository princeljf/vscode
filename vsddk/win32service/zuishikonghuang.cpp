#include "stdafx.h"
#include<Windows.h>

int _tmain(int argc, _TCHAR* argv[])
{
	//打开设备
	HANDLE handle = CreateFileA("\\\\.\\MyDevice1_link", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBoxA(0, "打开设备失败", "错误", 0);
		return 0;
	}
	unsigned char buffer[50] = { 0 };
	DWORD len;

	OVERLAPPED over = { 0 };
	HANDLE Event = CreateEvent(NULL, FALSE, FALSE, NULL);
	over.hEvent = Event;

	if (!ReadFile(handle, buffer, 49, &len, &over)){
		if (GetLastError() == ERROR_IO_PENDING){
			puts("I/O is Pending");
		}
	}

	Sleep(3000);
	CancelIo(handle);

	CloseHandle(handle);

	return 0;
}
