// InitSDKDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "InitSDKDlg.h"
#include "EVDemoCallBack.h"

HWND g_hWndInitSDK;

#if TEST_PRO
void CEVDemoCallBack::InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
	else
	{
		SendMessage(g_hWndInitSDK, WM_INITSDK_CALLBACK, NULL, NULL);
	}
}
#endif


IMPLEMENT_DYNAMIC(CInitSDKDlg, CDialogEx)

CInitSDKDlg::CInitSDKDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInitSDKDlg::IDD, pParent)
{
}

CInitSDKDlg::~CInitSDKDlg()
{
}

void CInitSDKDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CInitSDKDlg, CDialogEx)
	ON_BN_CLICKED(IDC_INIT_BTN, &CInitSDKDlg::OnBnClickedInitBtn)
END_MESSAGE_MAP()

// CInitSDKDlg 消息处理程序
void CInitSDKDlg::OnBnClickedInitBtn()
{
	CString strAppID; 
	CString strAccessKey;
	CString strSecretKey;
	CString strUserData;
	GetDlgItemText(IDC_EDIT_APPID, strAppID);
	GetDlgItemText(IDC_EDIT_ACCESSKEY, strAccessKey);
	GetDlgItemText(IDC_EDIT_SECRETKEY, strSecretKey);
	GetDlgItemText(IDC_EDIT_USERDATA, strUserData);

	AfxGetEVLive()->InitSDK(CT2CA(strAppID), CT2CA(strAccessKey), CT2CA(strSecretKey), CT2CA(strUserData));
}

LRESULT CInitSDKDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITSDK_CALLBACK:
		{
			CString strAppID;
			GetDlgItemText(IDC_EDIT_APPID, strAppID);
			APPID = strAppID.GetString();

			CString strUserData;
			GetDlgItemText(IDC_EDIT_USERDATA, strUserData);
			APP_USERNAME = strUserData.GetString();
			EndDialog(IDOK);
		}
		break;
	default:
		break;
	}
	return CDialogEx::WindowProc(uMsg, wParam, lParam);
}

BOOL CInitSDKDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	SetDlgItemText(IDC_EDIT_APPID, _T("evdemo"));
	SetDlgItemText(IDC_EDIT_ACCESSKEY, _T("test"));
	SetDlgItemText(IDC_EDIT_SECRETKEY, _T("helloworld"));
	SetDlgItemText(IDC_EDIT_USERDATA, _T("username"));

	g_hWndInitSDK = m_hWnd;
	return TRUE;
}