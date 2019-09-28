#include "NativeAPI.h"

ZWQUERYSYSTEMINFORMATION ZwQuerySystemInformation = NULL;
ZWQUERYVIRTUALMEMORY ZwQueryVirtualMemory = NULL;
ZWQUERYINFORMATIONTHREAD ZwQueryInformationThread = NULL;
NTQUERYOBJECT NtQueryObject = NULL;
NTSUSPENDPROCESS NtSuspendProcess = NULL;
NTRESUMEPROCESS NtResumeProcess = NULL;

BOOL InitNativeAPI()
{
	HMODULE hNtDll = GetModuleHandle("ntdll.dll");
	HMODULE hKernelDll = GetModuleHandle("kernel32.dll");
	if(NULL == hNtDll) return FALSE;
	if(NULL == hKernelDll) return FALSE;

	ZwQuerySystemInformation = (ZWQUERYSYSTEMINFORMATION)GetProcAddress(hNtDll, "ZwQuerySystemInformation");
	if(NULL == ZwQuerySystemInformation) return FALSE;
	ZwQueryVirtualMemory = (ZWQUERYVIRTUALMEMORY)GetProcAddress(hNtDll, "ZwQueryVirtualMemory");
	if(NULL == ZwQuerySystemInformation) return FALSE;
	ZwQueryInformationThread = (ZWQUERYINFORMATIONTHREAD)GetProcAddress(hNtDll, "ZwQueryInformationThread");
	if(NULL == ZwQueryInformationThread) return FALSE;
	NtQueryObject = (NTQUERYOBJECT)GetProcAddress(hNtDll, "NtQueryObject");
	if(NULL == NtQueryObject) return FALSE;
	NtSuspendProcess = (NTSUSPENDPROCESS)GetProcAddress(hNtDll, "NtSuspendProcess");
	if(NULL == NtSuspendProcess) return FALSE;
	NtResumeProcess = (NTRESUMEPROCESS)GetProcAddress(hNtDll, "NtResumeProcess");
	if(NULL == NtResumeProcess) return FALSE;

	return TRUE;
}