#include "Module.h"

BOOL EnumProcMoudles(PARM* Parm)
{
	DWORD ProcessId = Parm->parm1;
	DWORD BaseAddress = 0;

	BOOL bModuleFind = FALSE;
	DWORD ModuleBase = 0;
	DWORD ModuleSize = 0;
	// 打开进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, ProcessId);
	if(!hProcess) return FALSE;

	for(BaseAddress = 0; BaseAddress < 0x80000000; BaseAddress += 0x1000)
	{
		MEMORY_BASIC_INFORMATION mbi;
		memset(&mbi, 0, sizeof(MEMORY_BASIC_INFORMATION));

		if(NT_SUCCESS(ZwQueryVirtualMemory(hProcess, (PVOID)BaseAddress, MemoryBasicInformation, &mbi, sizeof(mbi), 0)))
		{
			LPVOID pModuleNameBuffer = NULL;
			PUNICODE_STRING SectionName = NULL;
			
			// 开始条件
			if (!bModuleFind && mbi.Type == MEM_IMAGE && mbi.AllocationBase == (PVOID)BaseAddress)
			{
				bModuleFind = TRUE;
				ModuleBase = (DWORD)mbi.AllocationBase;
			}

			// 结束条件
			if (mbi.Type != MEM_IMAGE && bModuleFind)
			{
				ModuleSize = (DWORD)mbi.BaseAddress - ModuleBase;
				bModuleFind = FALSE;
			}
			else
			{
				continue;
			}

			pModuleNameBuffer = malloc(MAX_PATH);
			if(NT_SUCCESS(ZwQueryVirtualMemory(hProcess, (PVOID)ModuleBase, MemorySectionName, pModuleNameBuffer, MAX_PATH, 0)))
			{
				SectionName = (PUNICODE_STRING)pModuleNameBuffer;
				wprintf(L"%s", SectionName->Buffer);
				printf(" 0x%x", ModuleBase);
				printf(" 0x%x", ModuleSize);
				printf("\n");
			}
			free(pModuleNameBuffer);
		}
	}

	return TRUE;
}