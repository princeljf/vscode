#include "SSDT.h"


VOID WPOFF()
{
	//WP CR0关闭内核页面保护
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
	//WP CR0恢复内核页面保护
	__asm
	{
		cli
			mov eax, cr0
			or eax, 10000h
			mov cr0, eax
			sti
	}
}
