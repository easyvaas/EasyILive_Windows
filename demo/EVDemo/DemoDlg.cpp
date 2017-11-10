
// DemoDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"

#include "Dialog/AddCameraDialog.h"
#include "Dialog/WndSelDlg.h"
#include "Dialog/AddWindowDialog.h"
#include "AGEventDef.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// 用于应用程序“关于”菜单项的 CAboutDlg 对话框
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// CDemoDlg 对话框
CDemoDlg::CDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDemoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pWindowOpts = new GameInfo[MAX_GAMEOPTS_COUNT];
	m_iOptsCount = 0;
}

CDemoDlg::~CDemoDlg()
{
	delete[]m_pWindowOpts;
	m_pWindowOpts = NULL;
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()

    ON_BN_CLICKED(IDC_SCREENSHOT_BUTTON, &CDemoDlg::OnBnClickedScreenshotButton)
    ON_BN_CLICKED(IDC_CAMERA_BUTTON, &CDemoDlg::OnBnClickedCameraButton)
	ON_BN_CLICKED(IDC_WINDOWS_BUTTON, &CDemoDlg::OnBnClickedWindowsButton)

	
	
END_MESSAGE_MAP()


// CDemoDlg 消息处理程序
BOOL CDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// 设置大图标;
	SetIcon(m_hIcon, FALSE);		// 设置小图标;

	// 连麦用户窗口;
	m_dlgChatUser.m_captureWnd = GetDlgItem(IDC_VIDEO_STATIC)->GetSafeHwnd();
	m_dlgChatUser.Create(IDD_CHATUSER_DIALOG, this);
	CRect rectChat;
	GetDlgItem(IDC_STATIC_CHAT_USER)->GetWindowRect(rectChat);
	rectChat.top -= 20;
	m_dlgChatUser.MoveWindow(rectChat);
	m_dlgChatUser.ShowWindow(SW_SHOW);

	// TODO:  在此添加额外的初始化代码;
	CWnd* pWndVideo = GetDlgItem(IDC_VIDEO_STATIC);
	// 此处视频创建显示比例最好控制为16：9
	m_dlgChatUser.m_hWndTestWnd = pWndVideo->GetSafeHwnd();

	return TRUE;
}

void CDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDemoDlg::OnBnClickedScreenshotButton()
{
	// 获取桌面窗口的CDC;
	HWND hWndDesktop = ::GetDesktopWindow();
	ASSERT(hWndDesktop);
	HDC hDcDesktop = ::GetDC(hWndDesktop);
	// 获取窗口的大小; 
	CRect rc;
	::GetClientRect(hWndDesktop, &rc);

	// 复制到图像;
	CImage image;
	image.Create(rc.Width(), rc.Height(), 24);
	HDC hDC = image.GetDC();
	BitBlt(hDC, 0, 0, rc.Width(), rc.Height(), hDcDesktop, 0, 0, SRCCOPY);
	image.ReleaseDC();
	::ReleaseDC(hWndDesktop, hDcDesktop);

	// 弹出窗口选择区域;
	MonitorsInfo infoMonitor;
	CRect rectSel(0, 0, 0, 0);
	CWndSelDlg dlg(image);
	int nRet = dlg.DoModal();
	if (IDOK == nRet)
	{
		rectSel = dlg.GetSelRect();
	}
	if (!rectSel.IsRectEmpty())
	{
		::SendMessage(g_ChatUserWnd, WM_MSGID(EID_SCREEN_CAPTURE_DESKTOP),0, (LPARAM)&rectSel);
// 		infoMonitor.rectMonitors.left = rectSel.left;
// 		infoMonitor.rectMonitors.top = rectSel.top;
// 		infoMonitor.rectMonitors.right = rectSel.right;
// 		infoMonitor.rectMonitors.bottom = rectSel.bottom;
// 		AfxGetEVLive()->AddScene(&infoMonitor);
	}
}


void CDemoDlg::OnBnClickedCameraButton()
{
    CAddCameraDialog cameraDlg;
	if (IDOK == cameraDlg.DoModal() && cameraDlg.bChangedVideoProfile)
	{
		::SendMessage(g_ChatUserWnd, WM_MSGID(EID_CHANED_VIDEOPROFILE), 0, 0);
	}

}

void CDemoDlg::OnBnClickedWindowsButton()
{
}

LRESULT CDemoDlg::WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITSDK_CALLBACK:
	case WM_INITINSTANCE_CALLBACK:
	case WM_INITVIDEOWND_CALLBACK:
	case WM_PREPARESTREAM_CALLBACK:
	case WM_ADDGAME_CALLBACK:
	case WM_MODIFYGAME_CALLBACK:
	case WM_ADDMONITOR_CALLBACK:
	case WM_ADDTEXT_CALLBACK:
	case WM_MODIFYTEXT_CALLBACK:
	case WM_ADDIMAGE_CALLBACK:
	case WM_MODIFYIMAGE_CALLBACK:
	case WM_ADDCAMERA_CALLBACK:
	case WM_MODIFYCAMERA_CALLBACK:
	{
			return 0;
		}
		break;
	
	default:
		break;
	}
	return CDialogEx::WindowProc(uMsg, wParam, lParam);
}
