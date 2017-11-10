
// Demo.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"		// 主符号
#include "DemoDlg.h"
#include "EVDemoCallBack.h"


// CDemoApp: 
// 有关此类的实现，请参阅 Demo.cpp
//
//#define TEST_ENV //测试环境
class CDemoApp : public CWinApp
{
public:
	CDemoApp();

// 重写
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 实现

	DECLARE_MESSAGE_MAP()
public:
	std::string app_id = "";
	std::string access_key = "";
	std::string seceret_key = "";
private:
	CEVDemoCallBack*	m_pCallBack;
};

extern CDemoApp theApp;