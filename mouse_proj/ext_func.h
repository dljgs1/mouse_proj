#include"stdafx.h"
HHOOK mouse_Hook;
LRESULT CALLBACK LowLevelMouseProc(INT nCode,
	WPARAM wParam,
	LPARAM lParam
	);
BOOL UninstallHook();  //ж��  
BOOL InstallHook();     //��װ