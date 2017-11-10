
// DemoDlg.cpp : ʵ���ļ�
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

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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

// CDemoDlg �Ի���
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


// CDemoDlg ��Ϣ�������
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

	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��;
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��;

	// �����û�����;
	m_dlgChatUser.m_captureWnd = GetDlgItem(IDC_VIDEO_STATIC)->GetSafeHwnd();
	m_dlgChatUser.Create(IDD_CHATUSER_DIALOG, this);
	CRect rectChat;
	GetDlgItem(IDC_STATIC_CHAT_USER)->GetWindowRect(rectChat);
	rectChat.top -= 20;
	m_dlgChatUser.MoveWindow(rectChat);
	m_dlgChatUser.ShowWindow(SW_SHOW);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������;
	CWnd* pWndVideo = GetDlgItem(IDC_VIDEO_STATIC);
	// �˴���Ƶ������ʾ������ÿ���Ϊ16��9
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
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CDemoDlg::OnBnClickedScreenshotButton()
{
	// ��ȡ���洰�ڵ�CDC;
	HWND hWndDesktop = ::GetDesktopWindow();
	ASSERT(hWndDesktop);
	HDC hDcDesktop = ::GetDC(hWndDesktop);
	// ��ȡ���ڵĴ�С; 
	CRect rc;
	::GetClientRect(hWndDesktop, &rc);

	// ���Ƶ�ͼ��;
	CImage image;
	image.Create(rc.Width(), rc.Height(), 24);
	HDC hDC = image.GetDC();
	BitBlt(hDC, 0, 0, rc.Width(), rc.Height(), hDcDesktop, 0, 0, SRCCOPY);
	image.ReleaseDC();
	::ReleaseDC(hWndDesktop, hDcDesktop);

	// ��������ѡ������;
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
