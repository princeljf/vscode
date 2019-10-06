#include <ntddk.h>
extern "C" VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
extern "C" NTSTATUS DefDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp);
//���Ƕ�����豸��չ
typedef struct _DEVICE_EXTENSION {
	UNICODE_STRING SymLinkName;//����������
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

//#pragma code_seg("INIT")
extern "C" NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
	DbgPrint("DriverEntry\r\n");

	pDriverObject->DriverUnload = DriverUnload;//ע������ж�غ���

	//ע����ǲ����
	pDriverObject->MajorFunction[IRP_MJ_CREATE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_CLOSE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_WRITE] = DefDispatchRoutine;
	pDriverObject->MajorFunction[IRP_MJ_READ] = ReadDispatchRoutine;

	NTSTATUS status;
	PDEVICE_OBJECT pDevObj;
	PDEVICE_EXTENSION pDevExt;

	//�����豸���Ƶ��ַ���
	UNICODE_STRING devName;
	RtlInitUnicodeString(&devName, L"\\Device\\MyDevice1");

	//�����豸
	status = IoCreateDevice(pDriverObject, sizeof(DEVICE_EXTENSION), &devName, FILE_DEVICE_UNKNOWN, 0, TRUE, &pDevObj);
	if (!NT_SUCCESS(status))
		return status;

	pDevObj->Flags |= DO_BUFFERED_IO;//���豸����Ϊ�����豸
	pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;//�õ��豸��չ

	//������������
	UNICODE_STRING symLinkName;
	RtlInitUnicodeString(&symLinkName, L"\\??\\MyDevice1_link");
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

VOID Read_CancelIRP(PDEVICE_OBJECT DeviceObject, PIRP pIrp)
{
	DbgPrint("Read_CancelIRP pIrp: 0x%X\r\n", pIrp);

	//���״̬����Ϊ STATUS_CANCELLED
	pIrp->IoStatus.Status = STATUS_CANCELLED;
	//�����ֽ���
	pIrp->IoStatus.Information = 0;
	//��� IRP
	IoCompleteRequest(pIrp, IO_NO_INCREMENT);

	//�ͷ� Cancel ������
	IoReleaseCancelSpinLock(pIrp->CancelIrql);
}


extern "C" NTSTATUS ReadDispatchRoutine(PDEVICE_OBJECT pDevObj, PIRP pIrp)
{
	DbgPrint("ReadDispatchRoutine\r\n");
	NTSTATUS status = STATUS_SUCCESS;

	//�õ��豸��չ
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)pDevObj->DeviceExtension;

	//�õ�I/O��ջ�ĵ�ǰ��һ�㣬Ҳ����IO_STACK_LOCATION�ṹ��ָ��
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(pIrp);

	//ULONG ReadLength = stack->Parameters.Read.Length;//�õ����ĳ���
	//ULONG ReadOffset = (ULONG)stack->Parameters.Read.ByteOffset.QuadPart;//�õ���ƫ����
	//DbgPrint("ReadLength: %d\r\nReadOffset: %d\r\n", ReadLength, ReadOffset);//��������Ϣ

	//PVOID buffer = pIrp->AssociatedIrp.SystemBuffer;//�õ�������ָ��

	//if (ReadOffset + ReadLength > BUFFER_LENGTH){
	//  //���Ҫ�����ĳ����˻���������ʧ�����IRP��������Ч
	//  DbgPrint("E: The size of the data is too long.\r\n");
	//  status = STATUS_FILE_INVALID;//�������û�ģʽ�µ�GetLastError
	//  ReadLength = 0;//���ò�����0�ֽ�
	//}
	//else{
	//  //û�г���,����л���������
	//  DbgPrint("OK, I will copy the buffer.\r\n");
	//  RtlMoveMemory(buffer, pDevExt->buffer + ReadOffset, ReadLength);
	//  status = STATUS_SUCCESS;
	//}

	IoSetCancelRoutine(pIrp, Read_CancelIRP);

	IoMarkIrpPending(pIrp);

	DbgPrint("IoMarkIrpPending pIrp: 0x%X\r\n", pIrp);
	return STATUS_PENDING;

	//pIrp->IoStatus.Status = status;//����IRP���״̬���������û�ģʽ�µ�GetLastError
	//pIrp->IoStatus.Information = ReadLength;//���ò����ֽ���
	//IoCompleteRequest(pIrp, IO_NO_INCREMENT);//���IRP
	//return status;
}
