// IFNtm.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CIFNtmApp:
// �йش����ʵ�֣������ IFNtm.cpp
//

class CIFNtmApp : public CWinApp
{
public:
	CIFNtmApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CIFNtmApp theApp;
