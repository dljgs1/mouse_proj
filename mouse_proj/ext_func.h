#include"stdafx.h"
HHOOK mouse_Hook;
LRESULT CALLBACK LowLevelMouseProc(INT nCode,
	WPARAM wParam,
	LPARAM lParam
	);
BOOL UninstallHook();  //Ð¶ÔØ  
BOOL InstallHook();     //°²×°