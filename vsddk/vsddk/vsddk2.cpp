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
	
	//创建通信设备：用于跟应用层通信
	status = IoCreateDevice(DriverObject,0,&DeviceName,FILE_DEVICE_UNKNOWN,0,FALSE,&DriverDeviceObject);
	if(!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverDeviceObject);
		return STATUS_NO_SUCH_DEVICE;
	}
	//同样我们也需要一个符号链接，不然会影响到驱动和应用层的通信
	status = IoCreateSymbolicLink(&DosDeviceName, &DeviceName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(DriverDeviceObject);
		return STATUS_NO_SUCH_DEVICE;
	}
	DriverObject->DriverUnload = DriverUnload;//驱动卸载例程
	//IRP_MJ_CREATE，响应的是应用层函数，应用层调用CreateFile()函数就会进入这个例程IODispatch
	DriverObject->MajorFunction[IRP_MJ_CREATE] = IODispatch;//CreateFile();
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = IODispatch;//CloseHandle();
	DriverObject->MajorFunction[IRP_MJ_READ] = IODispatch;//ReadFile();
	DriverObject->MajorFunction[IRP_MJ_WRITE] = IODispatch;//WriteFile();
	//一般跟应用层通信用IRP_MJ_DEVICE_CONTROL例程，对应的是应用层下的DeviceIoControl()
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IOManager;//DeviceIoControl();

	
	DbgPrint("vsddk2 驱动加载成功");
	return STATUS_SUCCESS;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	UNICODE_STRING DeviceName;
	UNICODE_STRING DosDeviceName;
	//删除符号链接
	RtlInitUnicodeString(&DosDeviceName, DOSDEVICE);
	IoDeleteSymbolicLink(&DosDeviceName);

	if (DriverDeviceObject != NULL)
	{
		IoDeleteDevice(DriverDeviceObject);
	}
	DbgPrint("vsddk2 驱动卸载成功");
}
//驱动与应用层IRP通信例程
NTSTATUS IOManager(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DbgPrint("IOManager");
	//获取通信IPR控制码
	PIO_STACK_LOCATION StackLocation = IoGetCurrentIrpStackLocation(Irp);



	Irp->IoStatus.Status = STATUS_SUCCESS;
	//完成例程，表示调用者已经完成了对指定请求的所有处理操作
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS IODispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	DbgPrint("IODispatch");
	Irp->IoStatus.Status = STATUS_SUCCESS;
	//完成例程，表示调用者已经完成了对指定请求的所有处理操作
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}