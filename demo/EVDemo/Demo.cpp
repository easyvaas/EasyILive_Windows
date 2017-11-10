﻿
// Demo.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"
#include "Demo.h"
#include "InitSDKDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//----------------------------

// CDemoApp
BEGIN_MESSAGE_MAP(CDemoApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()

#define PROFILE_BUFFER_MAXSIZE	(255)

// CDemoApp 构造
CDemoApp::CDemoApp()
{
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;
}

CDemoApp theApp;

BOOL CDemoApp::InitInstance()
{
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);
	CWinApp::InitInstance();
	AfxEnableControlContainer();
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	SetRegistryKey(_T("EVDemo"));
	// 必须提前初始化;
	HRESULT Hr = ::CoInitialize(NULL);
	if (FAILED(Hr))
	{
		return 0;
	}

	m_pCallBack = new CEVDemoCallBack();
	// 创建Instance;
	CEVLiveFactory::CreateInstance(m_pCallBack);

	//
#ifdef TEST_ENV
	app_id = "easyvaastest";
	access_key = "3hCPnGVLvyM";
	seceret_key = "YG1HdXr31VfbalXrs9ZKb6P91lH2T0zZ";
#else
	app_id = "53957274";
	access_key = "4K7kw4udnnq";
	seceret_key = "1ZHE3Oe6CA085YAW3yk3KTlx1otXsK2F";
#endif
#if TEST_PRO
	CInitSDKDlg dlgInitSDK;
	if (IDOK != dlgInitSDK.DoModal())
	{
		return FALSE;
	}
#else
	AfxGetEVLive()->InitSDK(const_cast<char*>(app_id.c_str()), const_cast<char*>(access_key.c_str()), const_cast<char*>(seceret_key.c_str()), "username");
#endif

	HINSTANCE hInstMain = AfxGetInstanceHandle();

	WNDCLASS wc;
	//（WinCE MFC默认对话框类名为Dialog，windows为#32770;
	::GetClassInfo(AfxGetInstanceHandle(), _T("#32770"), &wc);  
	wc.lpszClassName = EV_WINDOW_CLASS;
	AfxRegisterClass(&wc);

	CDemoDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	return TRUE;
}

int CDemoApp::ExitInstance()
{
	delete m_pCallBack;
	m_pCallBack = NULL;
	CEVLiveFactory::ReleaseInstance();

	::CoUninitialize();
	return CWinApp::ExitInstance();
}
