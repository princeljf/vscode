// ring3.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	printf("ring3 main.\r\n");
	//���豸
	HANDLE handle = CreateFile(L"\\\\.\\agp_link", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED | FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBoxA(0, "���豸ʧ��", "����", 0);
		return 0;
	}

	getchar();
	CloseHandle(handle);
	return 0;
}
