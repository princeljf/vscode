#include <ntddk.h>
extern "C" VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DefDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS WriteDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS QueryInfomationDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);

#define BUFFER_LENGTH 512 //缓冲区长度

//我们定义的设备扩展
typedef struct _DEVICE_EXTENSION {
	UNICODE_STRING SymLinkName;//符号链接名

	//这是为我们要处理读写请求而准备的缓冲区长度和指针
	ULONG filelength;//已经使用的长度（这个很像一个文件，故这样命名）
	PUCHAR buffer;//缓冲区指针
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//#pragma code_seg("INIT")	//蓝屏代码屏蔽
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	DbgPrint("DriverEntry\r\n");

	pDriverObject->DriverUnload = DriverUnload;//注册驱动卸载函数

	//注册派遣函数
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = WriteDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = ReadDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = QueryInfomationDispatchRoutine;

	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	//创建设备名称的字符串
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\lhh");

	//创建设备
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevObj);
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;//将设备设置为缓冲I/O设备
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//得到设备扩展

	//分配用于处理读写请求的缓冲区
	pDevExt->buffer = (PUCHAR)ExAllocatePool(PagedPool, BUFFER_LENGTH);
	//设置缓冲区已使用的大小
	pDevExt->filelength = 0;

	//内存清零
	RtlZeroMemory(pDevExt->buffer, BUFFER_LENGTH);

	//创建符号链接
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\lhh_link");
	pDevExt->SymLinkName = symLinkName;
	status = IoCreateSymbolicLink(&symLinkName, &devName);
	if (!NT_SUCCESS(status))
	{
		IoDeleteDevice(pDevObj);
		return status;
	}
	return STATUS_SUCCESS;
}

extern "C" VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
	DbgPrint("DriverUnload\r\n");
	PDEVICE_OBJECT pDevObj;
	pDevObj = pDriverObject->DeviceObject;

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//得到设备扩展

	//删除符号链接
	UNICODE_STRING pLinkName = pDevExt->SymLinkName;
	IoDeleteSymbolicLink(&pLinkName);

	//删除设备
	IoDeleteDevice(pDevObj);
}

extern "C" NTSTATUS DefDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("DefDispatchRoutine\r\n");
	NTSTATUS status = STATUS_SUCCESS;
	pIrp->IoStatus.Status = status;
	pIrp->IoStatus.Information = 0;
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);
	return status;
}
extern "C" NTSTATUS WriteDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("WriteDispatchRoutine\r\n");
	NTSTATUS status = STATUS_SUCCESS;

	//得到设备扩展
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//得到I/O堆栈的当前这一层，也就是IO_STACK_LOCATION结构的指针
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	ULONG WriteLength = stack->Parameters.Write.Length;//获取写入的长度
	ULONG WriteOffset = (ULONG)stack->Parameters.Write.ByteOffset.QuadPart;//获取写入的偏移量
	DbgPrint("WriteLength: %d\r\nWriteOffset: %d\r\n", WriteLength, WriteOffset);//输出相关信息

	PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;//得到缓冲区指针

	if (WriteOffset + WriteLength > BUFFER_LENGTH){
		//如果要操作的超出了缓冲区，则失败完成IRP，返回无效
		DbgPrint("E: The size of the data is too long.\r\n");
		status = STATUS_FILE_INVALID;
		WriteLength = 0;
	}
	else{
		//没有超出,则进行缓冲区复制，将写入的数据复制缓冲区
		memcpy(pDevExt->buffer + WriteOffset, buffer, WriteLength);
		status = STATUS_SUCCESS;

		//设置新的已经使用长度
		if (WriteLength + WriteOffset > pDevExt->filelength){
			pDevExt->filelength = WriteLength + WriteOffset;
		}
	}

	pIrp->IoStatus.Status = status;//设置IRP完成状态，会设置用户模式下的GetLastError
	pIrp->IoStatus.Information = WriteLength;//设置操作字节数
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//完成IRP
	return status;
}
extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("ReadDispatchRoutine\r\n");
	NTSTATUS status = STATUS_SUCCESS;

	//得到设备扩展
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//得到I/O堆栈的当前这一层，也就是IO_STACK_LOCATION结构的指针
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	ULONG ReadLength = stack->Parameters.Read.Length;//得到读的长度
	ULONG ReadOffset = (ULONG)stack->Parameters.Read.ByteOffset.QuadPart;//得到读偏移量
	DbgPrint("ReadLength: %d\r\nReadOffset: %d\r\n", ReadLength, ReadOffset);//输出相关信息

	PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;//得到缓冲区指针

	if (ReadOffset + ReadLength > BUFFER_LENGTH){
		//如果要操作的超出了缓冲区，则失败完成IRP，返回无效
		DbgPrint("E: The size of the data is too long.\r\n");
		status = STATUS_FILE_INVALID;//会设置用户模式下的GetLastError
		ReadLength = 0;//设置操作了0字节
	}
	else{
		//没有超出,则进行缓冲区复制
		DbgPrint("OK, I will copy the buffer.\r\n");
		RtlMoveMemory(buffer, pDevExt->buffer + ReadOffset, ReadLength);
		status = STATUS_SUCCESS;
	}

	pIrp->IoStatus.Status = status;//设置IRP完成状态，会设置用户模式下的GetLastError
	pIrp->IoStatus.Information = ReadLength;//设置操作字节数
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//完成IRP
	return status;
}
extern "C" NTSTATUS QueryInfomationDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("QueryInfomationDispatchRoutine\r\n");
	//用于处理应用程序GetFileSize获取文件大小（已经使用的大小）

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);//得到I/O堆栈的当前这一层，也就是IO_STACK_LOCATION结构的指针
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//得到设备扩展
	FILE_INFORMATION_CLASS fic = stack->Parameters.QueryFile.FileInformationClass;//得到FileInformationClass枚举类型
	if (fic == FileStandardInformation){
		PFILE_STANDARD_INFORMATION FileStandardInfo = (PFILE_STANDARD_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;//得到缓冲区指针
		FileStandardInfo->EndOfFile = RtlConvertLongToLargeInteger(pDevExt->filelength);//设置文件大小（已经使用的大小）
	}

	pIrp->IoStatus.Status = STATUS_SUCCESS;//设置IRP完成状态，会设置用户模式下的GetLastError
	pIrp->IoStatus.Information = stack->Parameters.QueryFile.Length;//设置操作字节数
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//完成IRP
	return STATUS_SUCCESS;
}