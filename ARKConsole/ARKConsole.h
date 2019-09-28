#ifndef _ARKCONSOLE_H_ 
#define _ARKCONSOLE_H_

#include <Windows.h>
#include <stdio.h>

typedef struct
{
	int parm1;
	int parm2;
}PARM;

typedef BOOL (*FnHandleCmd)(PARM* cmd);

typedef struct
{
	char szCmd[100];
	FnHandleCmd fun;
}REQUEST;


#endif // _ARKCONSOLE_H_