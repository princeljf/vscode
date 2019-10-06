#include "stdafx.h"

#define BUFFER_LENGTH 512 //����������

//���Ƕ�����豸��չ
typedef struct _DEVICE_EXTENSION {
	UNICODE_STRING DeviceNameLink;//����������
}DEVICE_EXTENSION, *PDEVICE_EXTENSION;

extern "C" void DriverUnload(PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DriverDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp);

PCWSTR gDEVICENAME = L"\\Device\\agp";
PCWSTR gDEVICENAMELINK = L"\\??\\agp_link";

extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pUnicodeString)
{
	NTSTATUS status = STATUS_SUCCESS;

	pDriverObject->DriverUnload = DriverUnload;
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DriverDispatch;	//CreateFile()
	pDriverObject->MajorFunction[IRP_MJ_READ] = DriverDispatch;		//ReadFile()
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DriverDispatch;	//WriteFile()
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverDispatch;	//CloseHandle()
	pDriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DriverDispatch;//DeviceIoControl()

	PDEVICE_OBJECT pDeviceObject = NULL;
	UNICODE_STRING DeviceName;
	RtlInitUnicodeString(&DeviceName, gDEVICENAME);
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &DeviceName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDeviceObject);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDeviceObject);
		return status;
	}
	
	UNICODE_STRING DeviceNameLink;
	RtlInitUnicodeString(&DeviceNameLink, gDEVICENAMELINK);
	status = IoCreateSymbolicLink(&DeviceNameLink, &DeviceName);	
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDeviceObject);
		return status;
	}
	pDeviceObject->Flags |= DO_BUFFERED_IO;//���豸����Ϊ�����豸
	PDEVICE_EXTENSION pDevExt;
	pDevExt = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;
	pDevExt->DeviceNameLink = DeviceNameLink;//�洢��������

	DbgPrint("�������سɹ�");
	return status;
}

extern "C" void DriverUnload(PDRIVER_OBJECT pDriverObject){
	PDEVICE_OBJECT pDeviceObject = pDriverObject->DeviceObject;
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDeviceObject->DeviceExtension;//�õ��豸��չ
	IoDeleteSymbolicLink(&(pDevExt->DeviceNameLink));
	if (pDeviceObject != NULL)
	{
		IoDeleteDevice(pDeviceObject);
	}
	DbgPrint("����ж�سɹ�");
}

extern "C" NTSTATUS DriverDispatch(PDEVICE_OBJECT pDeviceObject, PIRP pIrp)
{
	DbgPrint("DriverDispatch\r\n");
	NTSTATUS status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	return status;
}

