
// Demo.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������
#include "DemoDlg.h"
#include "EVDemoCallBack.h"


// CDemoApp: 
// �йش����ʵ�֣������ Demo.cpp
//
//#define TEST_ENV //���Ի���
class CDemoApp : public CWinApp
{
public:
	CDemoApp();

// ��д
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
public:
	std::string app_id = "";
	std::string access_key = "";
	std::string seceret_key = "";
private:
	CEVDemoCallBack*	m_pCallBack;
};

extern CDemoApp theApp;