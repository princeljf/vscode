#include "SSDT.h"


VOID WPOFF()
{
	//WP CR0�ر��ں�ҳ�汣��
	__asm
	{
		cli
			mov eax, cr0
			and eax, not 10000h
			mov cr0, eax
	}
}
VOID WPON()
{
	//WP CR0�ָ��ں�ҳ�汣��
	__asm
	{
		cli
			mov eax, cr0
			or eax, 10000h
			mov cr0, eax
			sti
	}
}
