#include "stdafx.h"

VOID DriverUnload(IN PDRIVER_OBJECT DriverObject);
NTSTATUS IOManager(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS IODispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp);

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING RegistryPath);
PCWSTR DEVICE = L"\\Device\\vsddk2";
PCWSTR DOSDEVICE = L"\\DosDevices\\vsddk2";
PDEVICE_OBJECT DriverDeviceObject = NULL;
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
	UNICODE_STRING DeviceName;
	UNICODE_STRING DosDeviceName;
	NTSTATUS status;

	RtlInitUnicodeString(&DeviceName, DEVICE);
	RtlInitUnicodeString(&DosDeviceName, DOSDEVICE);
	
	//����ͨ���豸�����ڸ�Ӧ�ò�ͨ��
	status = IoCreateDevice(DriverObject,0,&DeviceName,FILE_DEVICE_UNKNOWN,0,FALSE,&DriverDeviceObject);
	if(!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverDeviceObject);
		return STATUS_NO_SUCH_DEVICE;
	}
	//ͬ������Ҳ��Ҫһ���������ӣ���Ȼ��Ӱ�쵽������Ӧ�ò��ͨ��
	status = IoCreateSymbolicLink(&DosDeviceName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverDeviceObject);
		return STATUS_NO_SUCH_DEVICE;
	}
	DriverObject->DriverUnload = DriverUnload;//����ж������
	//IRP_MJ_CREATE����Ӧ����Ӧ�ò㺯����Ӧ�ò����CreateFile()�����ͻ�����������IODispatch
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IODispatch;//CreateFile();
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IODispatch;//CloseHandle();
	DriverObject->MajorFunction[IRP_MJ_READ] = IODispatch;//ReadFile();
	DriverObject->MajorFunction[IRP_MJ_WRITE] = IODispatch;//WriteFile();
	//һ���Ӧ�ò�ͨ����IRP_MJ_DEVICE_CONTROL���̣���Ӧ����Ӧ�ò��µ�DeviceIoControl()
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOManager;//DeviceIoControl();

	
	DbgPrint("vsddk2 �������سɹ�");
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING DeviceName;
	UNICODE_STRING DosDeviceName;
	//ɾ����������
	RtlInitUnicodeString(&DosDeviceName, DOSDEVICE);
	IoDeleteSymbolicLink(&DosDeviceName);

	if (DriverDeviceObject != NULL)
	{
		IoDeleteDevice(DriverDeviceObject);
	}
	DbgPrint("vsddk2 ����ж�سɹ�");
}
//������Ӧ�ò�IRPͨ������
NTSTATUS IOManager(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DbgPrint("IOManager");
	//��ȡͨ��IPR������
	PIO_STACK_LOCATION StackLocation = IoGetCurrentIrpStackLocation(Irp);



	Irp->IoStatus.Status = STATUS_SUCCESS;
	//������̣���ʾ�������Ѿ�����˶�ָ����������д������
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IODispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DbgPrint("IODispatch");
	Irp->IoStatus.Status = STATUS_SUCCESS;
	//������̣���ʾ�������Ѿ�����˶�ָ����������д������
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}