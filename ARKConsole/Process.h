#ifndef _PROCESS_H_
#define _PROCESS_H_
#include "ARKConsole.h"
#include "NativeAPI.h"

BOOL EnumProcess(PARM* Parm);
BOOL EnumProcThreads(PARM* Parm);
BOOL EnumProcHandle(PARM* Parm);
BOOL SupThread(PARM* Parm);
BOOL RumThread(PARM* Parm);
BOOL SupProcess(PARM* Parm);
BOOL RumProcess(PARM* Parm);
#endif // _PROCESS_H_