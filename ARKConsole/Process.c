#include "Process.h"

DWORD WINAPI GetThreadStartAddress(HANDLE hThread,DWORD dwTargetPID)
{
	NTSTATUS ntStatus;
	HANDLE hDupHandle;
	DWORD dwStartAddress;

	HANDLE hCurrentProcess = GetCurrentProcess();
	HANDLE hTargetProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwTargetPID);
	if(!DuplicateHandle(hCurrentProcess, hThread, hTargetProcess, &hDupHandle, THREAD_QUERY_INFORMATION, FALSE, 0)){
		SetLastError(ERROR_ACCESS_DENIED);

		return 0;
	}

	ntStatus = ZwQueryInformationThread(hDupHandle, ThreadQuerySetWin32StartAddress, &dwStartAddress, sizeof(DWORD), NULL);
	CloseHandle(hDupHandle);
	if(ntStatus != STATUS_SUCCESS) 
		return 0;

	return dwStartAddress;

}

BOOL EnumProcess(PARM* Parm)
{
	PSYSTEM_PROCESSES psp=NULL;
	PVOID pBuffer = NULL;
	DWORD dwNeedSize = 0;
	DWORD dwProcessCount = 0;
	// ��ȡһ����ʵ�Ĵ�С
	NTSTATUS status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &dwNeedSize);   
	if ( status == STATUS_INFO_LENGTH_MISMATCH ) 
	{   
		pBuffer = malloc(dwNeedSize);   
		status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, (PVOID)pBuffer, dwNeedSize, NULL);   
		if ( status == STATUS_SUCCESS )   
		{   
			psp = (PSYSTEM_PROCESSES)pBuffer;

			printf("PID  �߳��� �ڴ��С ������\n");
			while(1)
			{
				printf("%-4d", psp->ProcessId);
				printf(" %3d", psp->ThreadCount);   
				printf(" %8dKB", psp->VmCounters.WorkingSetSize/1024);
				if (psp->ProcessId == 0)
				{
					wprintf(L"  System Idle Process\n");
				}
				else
				{
					wprintf(L"  %s\n", psp->ProcessName.Buffer);
				}
				dwProcessCount++;
				if (!psp->NextEntryDelta)
				{
					break;
				}
				psp = (PSYSTEM_PROCESSES)((ULONG)psp + psp->NextEntryDelta );   
			}

			free(pBuffer);   
			pBuffer = NULL;   
		}
	}   
	printf("��������: %d\r\n", dwProcessCount);
	return TRUE;
}

BOOL EnumProcThreads(PARM* Parm)
{
	// ��ȡPID
	DWORD ProcessId = Parm->parm1;
	PSYSTEM_PROCESSES psp=NULL;
	PVOID pBuffer = NULL;
	DWORD dwNeedSize = 0;
	
	// ��ȡһ����ʵ�Ĵ�С
	NTSTATUS status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, NULL, 0, &dwNeedSize);   
	if ( status == STATUS_INFO_LENGTH_MISMATCH ) 
	{   
		pBuffer = malloc(dwNeedSize);   
		status = ZwQuerySystemInformation(SystemProcessesAndThreadsInformation, (PVOID)pBuffer, dwNeedSize, NULL);   
		if ( status == STATUS_SUCCESS )   
		{   
			psp = (PSYSTEM_PROCESSES)pBuffer;
			while(1)
			{
				// ���ƥ�䣬����߳���Ϣ
				if (ProcessId == psp->ProcessId)
				{
					DWORD dwThreadCount = 0;
					printf("TID     ��ʼ��ַ\r\n");
					for (;dwThreadCount < psp->ThreadCount; dwThreadCount++)
					{
						DWORD dwLasterror = 0;
						PVOID pStartAddress = NULL;  
 						HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, (DWORD)psp->Threads[dwThreadCount].ClientId.UniqueThread);  
						if (hThread != NULL)
						{
							ZwQueryInformationThread(hThread, ThreadQuerySetWin32StartAddress, &pStartAddress,sizeof(pStartAddress), NULL);  
						}
 						CloseHandle(hThread);

						printf("%d ", psp->Threads[dwThreadCount].ClientId.UniqueThread);
						printf("0x%08x\r\n", pStartAddress);
						// ����ж��߳������ĸ�ģ��
					}
					break;
				}

				if (!psp->NextEntryDelta)
				{
					break;
				}
				psp = (PSYSTEM_PROCESSES)((ULONG)psp + psp->NextEntryDelta );   
			}

			free(pBuffer);   
			pBuffer = NULL;   
		}
	}   
	return TRUE;
}

// ����ָ������
BOOL SupProcess(PARM* Parm)
{
	NTSTATUS status;
	DWORD ProcessId = Parm->parm1;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
	if(!hProcess) return FALSE;
	if (NULL == NtSuspendProcess) return FALSE;
	status = NtSuspendProcess(hProcess);

	if (status == STATUS_SUCCESS)
	{
		CloseHandle(hProcess);
		return TRUE;
	}
	CloseHandle(hProcess);
	printf("�����:%x\r\n", status);
	return FALSE;
}

// �ָ�����
BOOL RumProcess(PARM* Parm)
{
	NTSTATUS status;
	DWORD ProcessId = Parm->parm1;
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
	if(!hProcess) return FALSE;
	if (NULL == NtResumeProcess) return FALSE;
	status = NtResumeProcess(hProcess);

	if (status == STATUS_SUCCESS)
	{
		CloseHandle(hProcess);
		return TRUE;
	}
	CloseHandle(hProcess);
	printf("�����:%x\r\n", status);
	return FALSE;
}

// ����ָ�����̵��߳�
BOOL SupThread(PARM* Parm)
{
	DWORD dwTID = Parm->parm1;
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTID);  
	if (NULL == hThread)
	{
		return FALSE;
	}

	//Wow64SuspendThread
	SuspendThread(hThread);

	if (0 == GetLastError())
	{
		CloseHandle(hThread);
		return TRUE;
	}
	CloseHandle(hThread);
	printf("�����:%x\r\n", GetLastError());
	return FALSE;
}

// �ָ�ָ�����̵��߳�
BOOL RumThread(PARM* Parm)
{
	DWORD dwTID = Parm->parm1;
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, FALSE, dwTID);  
	if (NULL == hThread)
	{
		return FALSE;
	}
	//Wow64SuspendThread
	ResumeThread(hThread);

	if (0 == GetLastError())
	{
		CloseHandle(hThread);
		return TRUE;
	}
	CloseHandle(hThread);
	printf("�����:%x\r\n", GetLastError());
	return FALSE;
}

// ö�ٽ��̾��
BOOL EnumProcHandle(PARM* Parm)
{
	DWORD ProcessId = Parm->parm1;
	PSYSTEM_HANDLE_INFORMATION_EX pHandleInfo = NULL;
	POBJECT_NAME_INFORMATION pNameInfo;
	POBJECT_NAME_INFORMATION pNameType;
	PVOID pBuffer = NULL;
	char szType[128] = {0};
	char szName[512] = {0};
	DWORD dwIndex = 0;
	DWORD dwSize = 0x1000;
	NTSTATUS status;
	// ��ѯ�����Ϣ��ѭ�������ڴ棬ֱ������Ϊֹ
	do 
	{
		pBuffer = malloc(dwSize);
		memset(pBuffer, 0, dwSize);
		status = ZwQuerySystemInformation(SystemHandleInformation, pBuffer, dwSize, NULL);   
		if (status == STATUS_INFO_LENGTH_MISMATCH )
		{
			free(pBuffer);
			pBuffer = NULL;
			dwSize *= 2;
		}
	} while (status == STATUS_INFO_LENGTH_MISMATCH);
  
	if ( status == STATUS_SUCCESS )   
	{   
		pHandleInfo = (PSYSTEM_HANDLE_INFORMATION_EX)pBuffer;
		for(dwIndex = 0; dwIndex < pHandleInfo->NumberOfHandles; dwIndex++)
		{
			if (pHandleInfo->Information[dwIndex].ProcessId == ProcessId)
			{
				NtQueryObject((HANDLE)pHandleInfo->Information[dwIndex].Handle, ObjectNameInformation, szName, 512, NULL);
				NtQueryObject((HANDLE)pHandleInfo->Information[dwIndex].Handle, ObjectTypeInformation, szType, 128, NULL);
				pNameInfo = (POBJECT_NAME_INFORMATION)szName;
				pNameType = (POBJECT_NAME_INFORMATION)szType;
				wprintf(L"%s  %s\n", pNameType->Name.Buffer, pNameInfo->Name.Buffer);
			}
		}
		free(pBuffer);
		return TRUE;
	}

	return FALSE;
}

