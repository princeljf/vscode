#include <ntddk.h>
extern "C" VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DefDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS WriteDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS QueryInfomationDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);

#define BUFFER_LENGTH 512 //����������

//���Ƕ�����豸��չ
typedef struct _DEVICE_EXTENSION {
	UNICODE_STRING SymLinkName;//����������

	//����Ϊ����Ҫ�����д�����׼���Ļ��������Ⱥ�ָ��
	ULONG filelength;//�Ѿ�ʹ�õĳ��ȣ��������һ���ļ���������������
	PUCHAR buffer;//������ָ��
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//#pragma code_seg("INIT")	//������������
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	DbgPrint("DriverEntry\r\n");

	pDriverObject->DriverUnload = DriverUnload;//ע������ж�غ���

	//ע����ǲ����
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = WriteDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = ReadDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_QUERY_INFORMATION] = QueryInfomationDispatchRoutine;

	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	//�����豸���Ƶ��ַ���
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\lhh");

	//�����豸
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevObj);
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;//���豸����Ϊ����I/O�豸
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//�õ��豸��չ

	//�������ڴ����д����Ļ�����
	pDevExt->buffer = (PUCHAR)ExAllocatePool(PagedPool, BUFFER_LENGTH);
	//���û�������ʹ�õĴ�С
	pDevExt->filelength = 0;

	//�ڴ�����
	RtlZeroMemory(pDevExt->buffer, BUFFER_LENGTH);

	//������������
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

	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//�õ��豸��չ

	//ɾ����������
	UNICODE_STRING pLinkName = pDevExt->SymLinkName;
	IoDeleteSymbolicLink(&pLinkName);

	//ɾ���豸
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

	//�õ��豸��չ
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//�õ�I/O��ջ�ĵ�ǰ��һ�㣬Ҳ����IO_STACK_LOCATION�ṹ��ָ��
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	ULONG WriteLength = stack->Parameters.Write.Length;//��ȡд��ĳ���
	ULONG WriteOffset = (ULONG)stack->Parameters.Write.ByteOffset.QuadPart;//��ȡд���ƫ����
	DbgPrint("WriteLength: %d\r\nWriteOffset: %d\r\n", WriteLength, WriteOffset);//��������Ϣ

	PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;//�õ�������ָ��

	if (WriteOffset + WriteLength > BUFFER_LENGTH){
		//���Ҫ�����ĳ����˻���������ʧ�����IRP��������Ч
		DbgPrint("E: The size of the data is too long.\r\n");
		status = STATUS_FILE_INVALID;
		WriteLength = 0;
	}
	else{
		//û�г���,����л��������ƣ���д������ݸ��ƻ�����
		memcpy(pDevExt->buffer + WriteOffset, buffer, WriteLength);
		status = STATUS_SUCCESS;

		//�����µ��Ѿ�ʹ�ó���
		if (WriteLength + WriteOffset > pDevExt->filelength){
			pDevExt->filelength = WriteLength + WriteOffset;
		}
	}

	pIrp->IoStatus.Status = status;//����IRP���״̬���������û�ģʽ�µ�GetLastError
	pIrp->IoStatus.Information = WriteLength;//���ò����ֽ���
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//���IRP
	return status;
}
extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("ReadDispatchRoutine\r\n");
	NTSTATUS status = STATUS_SUCCESS;

	//�õ��豸��չ
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//�õ�I/O��ջ�ĵ�ǰ��һ�㣬Ҳ����IO_STACK_LOCATION�ṹ��ָ��
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	ULONG ReadLength = stack->Parameters.Read.Length;//�õ����ĳ���
	ULONG ReadOffset = (ULONG)stack->Parameters.Read.ByteOffset.QuadPart;//�õ���ƫ����
	DbgPrint("ReadLength: %d\r\nReadOffset: %d\r\n", ReadLength, ReadOffset);//��������Ϣ

	PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;//�õ�������ָ��

	if (ReadOffset + ReadLength > BUFFER_LENGTH){
		//���Ҫ�����ĳ����˻���������ʧ�����IRP��������Ч
		DbgPrint("E: The size of the data is too long.\r\n");
		status = STATUS_FILE_INVALID;//�������û�ģʽ�µ�GetLastError
		ReadLength = 0;//���ò�����0�ֽ�
	}
	else{
		//û�г���,����л���������
		DbgPrint("OK, I will copy the buffer.\r\n");
		RtlMoveMemory(buffer, pDevExt->buffer + ReadOffset, ReadLength);
		status = STATUS_SUCCESS;
	}

	pIrp->IoStatus.Status = status;//����IRP���״̬���������û�ģʽ�µ�GetLastError
	pIrp->IoStatus.Information = ReadLength;//���ò����ֽ���
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//���IRP
	return status;
}
extern "C" NTSTATUS QueryInfomationDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("QueryInfomationDispatchRoutine\r\n");
	//���ڴ���Ӧ�ó���GetFileSize��ȡ�ļ���С���Ѿ�ʹ�õĴ�С��

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);//�õ�I/O��ջ�ĵ�ǰ��һ�㣬Ҳ����IO_STACK_LOCATION�ṹ��ָ��
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//�õ��豸��չ
	FILE_INFORMATION_CLASS fic = stack->Parameters.QueryFile.FileInformationClass;//�õ�FileInformationClassö������
	if (fic == FileStandardInformation){
		PFILE_STANDARD_INFORMATION FileStandardInfo = (PFILE_STANDARD_INFORMATION)pIrp->AssociatedIrp.SystemBuffer;//�õ�������ָ��
		FileStandardInfo->EndOfFile = RtlConvertLongToLargeInteger(pDevExt->filelength);//�����ļ���С���Ѿ�ʹ�õĴ�С��
	}

	pIrp->IoStatus.Status = STATUS_SUCCESS;//����IRP���״̬���������û�ģʽ�µ�GetLastError
	pIrp->IoStatus.Information = stack->Parameters.QueryFile.Length;//���ò����ֽ���
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);//���IRP
	return STATUS_SUCCESS;
}