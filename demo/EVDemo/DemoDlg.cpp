
// DemoDlg.cpp : 实现文件
//
#include "stdafx.h"
#include "Demo.h"
#include "DemoDlg.h"

#include "Dialog/AddCameraDialog.h"
#include "Dialog/AddGameDialog.h"
#include "Dialog/AddImageDialog.h"
#include "Dialog/AddTextDialog.h"
#include "Dialog/WndSelDlg.h"
#include "Dialog/SetDlg.h"
#include "Dialog/AddWindowDialog.h"

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
}

void CDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MICROP_VOLUME_SLIDER, m_ctrMicrop);
	DDX_Control(pDX, IDC_DESTOP_VOLUME_SLIDER, m_ctrDestop);
	DDX_Control(pDX, IDC_EDIT_PUSHURL, m_edtPushUrl);
}

BEGIN_MESSAGE_MAP(CDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_HSCROLL()
    ON_BN_CLICKED(IDC_GAME_BUTTON, &CDemoDlg::OnBnClickedGameButton)
    ON_BN_CLICKED(IDC_SCREENSHOT_BUTTON, &CDemoDlg::OnBnClickedScreenshotButton)
    ON_BN_CLICKED(IDC_CAMERA_BUTTON, &CDemoDlg::OnBnClickedCameraButton)
    ON_BN_CLICKED(IDC_TEXT_BUTTON, &CDemoDlg::OnBnClickedTextButton)
    ON_BN_CLICKED(IDC_IMAGE_BUTTON, &CDemoDlg::OnBnClickedImageButton)
    ON_BN_CLICKED(IDC_STARTLIVE_BUTTON, &CDemoDlg::OnBnClickedStartliveButton)
    ON_BN_CLICKED(IDC_ENDLIVE_BUTTON, &CDemoDlg::OnBnClickedEndliveButton)
	ON_BN_CLICKED(IDC_SCENE_MOVEUP_BUTTON, &CDemoDlg::OnBnClickedSceneMoveupButton)
	ON_BN_CLICKED(IDC_SCENE_MOVEDOWM_BUTTON, &CDemoDlg::OnBnClickedSceneMovedowmButton)
	ON_BN_CLICKED(IDC_SCENE_MOVETOP_BUTTON, &CDemoDlg::OnBnClickedSceneMovetopButton)
	ON_BN_CLICKED(IDC_SCENE_MOVEBOTTOM_BUTTON, &CDemoDlg::OnBnClickedSceneMovebottomButton)
	ON_BN_CLICKED(IDC_SCENE_FULL_BUTTON, &CDemoDlg::OnBnClickedSceneFullButton)
	ON_BN_CLICKED(IDC_SCENE_SET_BUTTON, &CDemoDlg::OnBnClickedSceneSetButton)
	ON_BN_CLICKED(IDC_SCENE_DELETE_BUTTON, &CDemoDlg::OnBnClickedSceneDeleteButton)
	ON_BN_CLICKED(IDC_SET_BUTTON, &CDemoDlg::OnBnClickedSetButton)
	ON_BN_CLICKED(IDC_WINDOWS_BUTTON, &CDemoDlg::OnBnClickedWindowsButton)
	ON_BN_CLICKED(IDC_WHITE_BUTTON, &CDemoDlg::OnBnClickedWhiteButton)
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
	AfxGetEVLive()->InitVideoWnd(m_hWnd, pWndVideo->GetSafeHwnd(), true);
	m_ctrMicrop.SetPos((int)AfxGetEVLive()->GetMicropVolume());
	m_ctrDestop.SetPos((int)AfxGetEVLive()->GetDesktopVolume());

	EVStreamerParameter para;
	AfxGetEVLive()->SetParameter(para);

	// 消息窗口;
	m_dlgMessage.Create(IDD_MESSAGE_DIALOG, this);
	CRect rect;
	GetDlgItem(IDC_STATIC_MESSAGE_AREA)->GetWindowRect(rect);
	m_dlgMessage.MoveWindow(rect);
	m_dlgMessage.ShowWindow(SW_SHOW);

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

void CDemoDlg::OnBnClickedGameButton()
{
    // TODO:  在此添加控件通知处理程序代码
    CAddGameDialog gameDlg;
    gameDlg.DoModal();
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
		infoMonitor.rectMonitors.left = rectSel.left;
		infoMonitor.rectMonitors.top = rectSel.top;
		infoMonitor.rectMonitors.right = rectSel.right;
		infoMonitor.rectMonitors.bottom = rectSel.bottom;
		AfxGetEVLive()->AddScene(&infoMonitor);
	}
}


void CDemoDlg::OnBnClickedCameraButton()
{
    CAddCameraDialog cameraDlg;
    cameraDlg.DoModal();
}


void CDemoDlg::OnBnClickedTextButton()
{
    CAddTextDialog textDlg;
    textDlg.DoModal();
}


void CDemoDlg::OnBnClickedImageButton()
{
    CAddImageDialog imgDlg;
    imgDlg.DoModal();
}


void CDemoDlg::OnBnClickedStartliveButton()
{
	AfxGetEVLive()->PrepareStream();

// 	CUrlString strCmd;
// 	CString strUrl = VIDEO_URL;
// 	strUrl.Append(_T("/genstream?"));
// 	strCmd.SetUrl(strUrl.GetString());
// 	strCmd.AddPara(_T("appid"), APPID);
// 
// 	CmdItem cmdItem;
// 	cmdItem.cmdInfo.strCmd = strCmd.GetUrlString();
// 	cmdItem.pFunc = CDemoDlg::GetLID_CallBack;
// 	AflGetCmdManager()->AddCmd(cmdItem);
}


void CDemoDlg::OnBnClickedEndliveButton()
{
	AfxGetEVLive()->StopStream();

	GetDlgItem(IDC_STARTLIVE_BUTTON)->EnableWindow(TRUE);
	GetDlgItem(IDC_ENDLIVE_BUTTON)->EnableWindow(FALSE);

	GetDlgItem(IDC_SET_BUTTON)->EnableWindow(TRUE);
	m_edtPushUrl.SetWindowText(_T(""));

	SetWindowText(_T("Demo"));
}

void CDemoDlg::OnBnClickedSceneMoveupButton()
{
	AfxGetEVLive()->SetSelItemState(SceneItemsState_MoveUp);
}

void CDemoDlg::OnBnClickedSceneMovedowmButton()
{
	AfxGetEVLive()->SetSelItemState(SceneItemsState_MoveDown);
}

void CDemoDlg::OnBnClickedSceneMovetopButton()
{
	AfxGetEVLive()->SetSelItemState(SceneItemsState_MoveToTop);
}

void CDemoDlg::OnBnClickedSceneMovebottomButton()
{
	AfxGetEVLive()->SetSelItemState(SceneItemsState_MoveToBottom);
}

void CDemoDlg::OnBnClickedSceneFullButton()
{
	AfxGetEVLive()->SetSelItemState(SceneItemsState_MoveToFull);
}

void CDemoDlg::OnBnClickedSceneSetButton()
{
	char* pClass = new char[100];
	AfxGetEVLive()->GetSelSceneClass(pClass);
	if ( 0 == strcmp(pClass, DEVICE_CAPTURE) )
	{
		CAddCameraDialog dlgAddCamera(SCENE_MODIFY);
		dlgAddCamera.DoModal();
	}
	else if (0 == strcmp(pClass, WINDOW_CAPTURE))
	{
		CAddWindowDialog dlgAddWindow(SCENE_MODIFY);
		dlgAddWindow.DoModal();
	}
	else if ( 0 == strcmp(pClass, BITMAP_IMAGESOURCE) )
	{
		CAddImageDialog dlgAddCamera(SCENE_MODIFY);
		dlgAddCamera.DoModal();
	}
	else if ( 0 == strcmp(pClass, GRAPHICS_CAPTURE) )
	{
		CAddGameDialog dlgAddCamera(SCENE_MODIFY);
		dlgAddCamera.DoModal();
	}
	else if ( 0 == strcmp(pClass, TEXT_SOURCE) )
	{
		CAddTextDialog dlgAddCamera(SCENE_MODIFY);
		dlgAddCamera.DoModal();
	}
	delete pClass;
	pClass = NULL;
}

void CDemoDlg::OnBnClickedSceneDeleteButton()
{
	AfxGetEVLive()->DeleteSelItem();
}

void CDemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (((CScrollBar*)&m_ctrMicrop) == pScrollBar)
	{
		int iPos = m_ctrMicrop.GetPos();
		AfxGetEVLive()->SetMicropVolume(iPos);
	} 
	else if (((CScrollBar*)&m_ctrDestop) == pScrollBar)
	{
		int iPos = m_ctrDestop.GetPos();
		AfxGetEVLive()->SetDesktopVolume(iPos);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
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
			bool bSucess = (bool)wParam;
			CString strAddress = (wchar_t*)lParam;

			if (bSucess)
			{
				if (WM_PREPARESTREAM_CALLBACK == uMsg)
				{
					m_edtPushUrl.SetWindowText(strAddress);
					AfxGetEVLive()->StartStream();
					GetDlgItem(IDC_STARTLIVE_BUTTON)->EnableWindow(FALSE);
					GetDlgItem(IDC_ENDLIVE_BUTTON)->EnableWindow(TRUE);

					GetDlgItem(IDC_SET_BUTTON)->EnableWindow(FALSE);

					EVStreamerParameter para;
					AfxGetEVLive()->GetParameter(para);

					CString strTitle;
					strTitle.Format(_T("Demo: 分辨率-%d*%d, FPS-%d, 码率-%d"), para.videoResolutionWidth, para.videoResolutionHeight, para.frameRate, para.maxVideoBitrate);
					SetWindowText(strTitle);
				}
			} 
			else
			{
				MessageBox(strAddress);
			}
			return 0;
		}
		break;
	case WM_VEDIOWND_RBUTTONUP:
		{
			return VedioOnButtonUp(uMsg, wParam, lParam);
		}
		break;
	default:
		break;
	}
	return CDialogEx::WindowProc(uMsg, wParam, lParam);
}

LRESULT CDemoDlg::VedioOnButtonUp(UINT uMessage, WPARAM wParam, LPARAM lParam)
{
	//::AfxMessageBox(_T("右键菜单"));
	return 1;
}

void CDemoDlg::OnBnClickedSetButton()
{
	CSetDlg dlgSet;
	dlgSet.DoModal();
}

// 窗口;
void CDemoDlg::OnBnClickedWindowsButton()
{
	CAddWindowDialog dlgWindow;
	dlgWindow.DoModal();
}

// 白板;
void CDemoDlg::OnBnClickedWhiteButton()
{
	::MessageBox(m_hWnd, _T("该功能暂不提供"), _T("提示"), MB_ICONWARNING | MB_OK);
}
