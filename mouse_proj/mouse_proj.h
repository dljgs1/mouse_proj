
// mouse_proj.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cmouse_projApp: 
// �йش����ʵ�֣������ mouse_proj.cpp
//

class Cmouse_projApp : public CWinApp
{
public:
	Cmouse_projApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cmouse_projApp theApp;