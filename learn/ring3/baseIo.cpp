#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//打开设备
	HANDLE handle = CreateFileA("\\\\.\\lhh_link", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBoxA(0, "打开设备失败", "错误", 0);
		return 0;
	}
	unsigned char buffer1[50] = { 0 };
	unsigned char buffer2[50] = { 0 };
	DWORD len;

	//测试1：写入超出驱动申请的缓冲区大小的数据
	if (!WriteFile(handle, buffer1, 1000, &len, NULL))
		printf("1: failed\n");//当然会失败

	//测试2：写入字符串hello, driver，偏移量为5
	//也就是说，跳过前五个字节再写入
	sprintf((char*)buffer1, "hello, driver\r\n");
	OVERLAPPED ol = { 0 };
	ol.Offset = 5;
	if (WriteFile(handle, buffer1, strlen((char*)buffer1), &len, &ol)){
		printf("2: len: %d\n", len);
	}

	//测试3： 读出0-48（共49字节）的数据，并使用16进制输出
	if (ReadFile(handle, buffer2, 49, &len, NULL)){
		printf("3: len: %d\n", len);
		for (int i = 0; i < len; i++){
			printf("0x%02X ", buffer2[i]);
		}
	}

	//测试4： 获取驱动缓冲区已使用的大小（抽象成文件大小）
	printf("used: %d\n", GetFileSize(handle, NULL));

	getchar();
	CloseHandle(handle);
	return 0;
}