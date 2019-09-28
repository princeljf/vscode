#include "ring0.h"


NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pUnicodeString)
{


	DbgPrint("ring0 DriverEntry.");
	return STATUS_SUCCESS;
}

void DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("ring0 DriverUnload.");
}