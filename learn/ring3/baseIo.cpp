#include "stdafx.h"

int _tmain(int argc, _TCHAR* argv[])
{
	//���豸
	HANDLE handle = CreateFileA("\\\\.\\lhh_link", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handle == INVALID_HANDLE_VALUE){
		MessageBoxA(0, "���豸ʧ��", "����", 0);
		return 0;
	}
	unsigned char buffer1[50] = { 0 };
	unsigned char buffer2[50] = { 0 };
	DWORD len;

	//����1��д�볬����������Ļ�������С������
	if (!WriteFile(handle, buffer1, 1000, &len, NULL))
		printf("1: failed\n");//��Ȼ��ʧ��

	//����2��д���ַ���hello, driver��ƫ����Ϊ5
	//Ҳ����˵������ǰ����ֽ���д��
	sprintf((char*)buffer1, "hello, driver\r\n");
	OVERLAPPED ol = { 0 };
	ol.Offset = 5;
	if (WriteFile(handle, buffer1, strlen((char*)buffer1), &len, &ol)){
		printf("2: len: %d\n", len);
	}

	//����3�� ����0-48����49�ֽڣ������ݣ���ʹ��16�������
	if (ReadFile(handle, buffer2, 49, &len, NULL)){
		printf("3: len: %d\n", len);
		for (int i = 0; i < len; i++){
			printf("0x%02X ", buffer2[i]);
		}
	}

	//����4�� ��ȡ������������ʹ�õĴ�С��������ļ���С��
	printf("used: %d\n", GetFileSize(handle, NULL));

	getchar();
	CloseHandle(handle);
	return 0;
}