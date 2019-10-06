#include "stdafx.h"

#include "test.h"


extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath)
{
	NTSTATUS status = STATUS_SUCCESS;
	DbgPrint("DriverEntry Success.\r\n");
	DriverObject->DriverUnload = DriverUnload;

	return status;
};

VOID DriverUnload (IN PDRIVER_OBJECT DriverObject)
{
	DbgPrint("DriverUnload Success.\r\n");
};