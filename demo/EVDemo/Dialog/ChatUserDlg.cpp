#include "stdafx.h"
#include "ChatUserDlg.h"
#include "AGEventDef.h"
#include "EVDemoCallBack.h"
#include "JsonValueEx.h"
#include "Demo.h"
#include <atlconv.h>

#define IDC_BASEWND_VIDEO               113
#define	VIDEO_RATIO						(9.0 / 16.0)		// width / height

HWND g_ChatUserWnd = NULL;


CCompositionLayout::CCompositionLayout()
{
	m_vec_uid.clear();
}

CCompositionLayout::~CCompositionLayout()
{

}

void CCompositionLayout::SetCanvasAttribute(int width, int height, const char* bgColor, const char* appData, int appDataLen )
{
	m_layout.canvasWidth     = width;    //窗口宽度
	m_layout.canvasHeight    = height;   //窗口高度
	m_layout.backgroundColor = bgColor;
	m_layout.appData         = appData;
	m_layout.appDataLength   = appDataLen;
	m_width  = width;
	m_height = height;
}

void CCompositionLayout::UserJoinChannel(unsigned int uid)
{
	m_lock.Lock();
	m_vec_uid.push_back(uid);
	_SetCompositionLayout();
	m_lock.UnLock();
	//m_vec_uid.push_back()
}

void CCompositionLayout::Clear()
{
	m_vec_uid.clear();
}

void CCompositionLayout::UserLeaveChannel(unsigned int uid)
{
	m_lock.Lock();
	bool bAssignNextValue = false;
	for (auto iter = m_vec_uid.begin(); iter != m_vec_uid.end(); ++iter)
	{
		if (uid == *iter)
		{
			m_vec_uid.erase(iter);
			break;
		}
	}

	_SetCompositionLayout();
	m_lock.UnLock();
}

// 三行两列，纵向占满全部高度，横向最多两列，所以先计算高度
void CCompositionLayout::_SetCompositionLayout()
{
	m_layout.regionCount = m_vec_uid.size();
	
	//横纵间距都是20像素
	int distance = 20;
	double distance_rate = 20 / (double)m_height;
	
	double h = (m_height - (max_rows + 1) * 20) / (double)max_rows;
    
	//double w = (m_width - (max_cols + 1) * 20) / (double)max_cols;
	
	double rate_h = (double)h/m_height;
	double rate_w = rate_h;
	

	for (int i = 0; i < m_vec_uid.size(); ++i)
	{
		regions[i].uid         = m_vec_uid[i];
		regions[i].alpha       = 1.0;
	
		regions[i].renderMode  = EV_RENDER_MODE_FIT;
		if (0 == i)
		{
			regions[i].width   = 1.0;
			regions[i].height  = 1.0;
			regions[i].x       = 0;
			regions[i].y       = 0;
			regions[i].zOrder  = 0;
		}
		else
		{
		    regions[i].width   = rate_w;
			regions[i].height  = rate_h;
			regions[i].x       = 1.0 - ((i-1)/rows+1)*(distance_rate+rate_w);
			regions[i].y       = ((i - 1) % rows + 1)*(distance_rate)+((i - 1) % rows)*rate_h;
			regions[i].zOrder  = 1;
		
		}
		//m_layout.regions->uid = m_vec_uid[i];

	}
	m_layout.regions = regions;
	AfxGetEVLive()->SetVideoCompositingLayout(m_layout);
}



IMPLEMENT_DYNAMIC(CChatUserDlg, CDialogEx)

CChatUserDlg::CChatUserDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CChatUserDlg::IDD, pParent)
{
	
	app_id = "evdev";//theApp.app_id;//theApp.app_id;
}

CChatUserDlg::~CChatUserDlg()
{
}

void CChatUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHANNEL, m_editChatRoom);
	DDX_Control(pDX, IDC_COMBO_ROLE, m_cmbRole);

	DDX_Control(pDX, IDC_CHECK_CONFIGUREPUBLISHER, m_chkConfigurePublisher);
	DDX_Control(pDX, IDC_EDIT_PUSH_URL_TO_OTHER, m_edtPushUrlToOther);
	DDX_Control(pDX, IDC_STATIC_JOINCHANNEL_INFO, m_staJoinChannelInfo);
	DDX_Control(pDX, IDC_BUTTON_JOIN_CHANNEL, m_btnJoinChannel);
	DDX_Control(pDX, IDC_BUTTON_LEAVE_CHANNEL, m_btnLeaveChannel);
}


BEGIN_MESSAGE_MAP(CChatUserDlg, CDialogEx)
	ON_MESSAGE(WM_ADDTOLIVE, &CChatUserDlg::OnAddToLive)
	
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CChatUserDlg::OnJoinChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CChatUserDlg::OnLeaveChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_REJOINCHANNEL_SUCCESS), &CChatUserDlg::OnReJoinChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), &CChatUserDlg::OnEIDFirstLocalFrame)
	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), &CChatUserDlg::OnEIDFirstRemoteFrameDecoded)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CChatUserDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STAT), &CChatUserDlg::OnRemoteVideoStat)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CChatUserDlg::OnEIDUserJoined)

	ON_MESSAGE(WM_MSGID(EID_EV_CREATE_CHANNEL), &CChatUserDlg::OnEIDCreateChannel)
	ON_MESSAGE(WM_MSGID(EID_EV_USER_AUTH), &CChatUserDlg::OnEIDUserAuth)
	
	ON_MESSAGE(WM_MSGID(EID_EV_USER_JOIN), &CChatUserDlg::OnEIDEVJoinChannel)
	ON_MESSAGE(WM_MSGID(EID_EV_USER_LEAVE), &CChatUserDlg::OnEIDEVLeaveChannel)

	ON_MESSAGE(WM_MSGID(EID_EV_BAD_NETWORK), &CChatUserDlg::OnEIDBadNetWork)


	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOIN_CHANNEL, &CChatUserDlg::OnJoinChannel)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &CChatUserDlg::OnLeaveChannel)
	ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CChatUserDlg::OnSelchangeComboRole)
	ON_BN_CLICKED(IDC_CHECK_CONFIGUREPUBLISHER, &CChatUserDlg::OnBnClickedCheckConfigurepublisher)
END_MESSAGE_MAP()


// CChatUserDlg 消息处理程序


LRESULT CChatUserDlg::OnJoinChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;
	m_listWndInfo.RemoveAll();

	//m_uid = lpData->uid;
	m_set_uid.insert(lpData->uid);
	
	if (m_user_choose_role == EVLIVE_USER_ROLE_AUDIENCE)//连麦请求者
		m_staJoinChannelInfo.SetWindowText(_T("连麦请求者进入频道成功!通知业务服务器"));
	else
	{
		m_staJoinChannelInfo.SetWindowText(_T("主播创建频道成功，并且成功进入频道!通知业务服务器"));
	}

	AfxGetEVLive()->JoinEVChannel(app_id.c_str(), lpData->channel, lpData->uid, m_user_choose_role);


	
	m_bJoinChannel = true;
	//m_btnJoinChannel.EnableWindow(false);
	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		EVStreamerParameter evParam;
		AfxGetEVLive()->GetParameter(evParam);
		m_remote_layout.SetCanvasAttribute(evParam.videoResolutionWidth, evParam.videoResolutionHeight);
		m_remote_layout.UserJoinChannel(lpData->uid);
	}

	delete[] lpData->channel;
	lpData->channel = NULL;
	delete lpData;
	lpData = NULL;
	return 0;
}

LRESULT CChatUserDlg::OnLeaveChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_LEAVE_CHANNEL lpData = (LPAGE_LEAVE_CHANNEL)wParam;
	m_listWndInfo.RemoveAll();
	m_bJoinChannel = false;
	auto iter = m_set_uid.find(m_uid);
	m_set_uid.erase(iter);
	//离开频道
	AfxGetEVLive()->LeaveEVChannel(app_id.c_str(), m_channel_id.c_str(), m_uid);
	// 离开后将背景图改成默认;
	SetDefaultBkg();

	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		EVStreamerParameter evParam;
		AfxGetEVLive()->GetParameter(evParam);
		m_remote_layout.SetCanvasAttribute(evParam.videoResolutionWidth, evParam.videoResolutionHeight);

		m_remote_layout.UserLeaveChannel(m_uid);
	}

	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnReJoinChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REJOINCHANNEL_SUCCESS lpData = (LPAGE_REJOINCHANNEL_SUCCESS)wParam;

	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnEIDFirstLocalFrame(WPARAM wParam, LPARAM lParam)
{
	LPAGE_FIRST_LOCAL_VIDEO_FRAME lpData = (LPAGE_FIRST_LOCAL_VIDEO_FRAME)wParam;

	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnEIDFirstRemoteFrameDecoded(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	LPAGE_FIRST_REMOTE_VIDEO_DECODED lpData = (LPAGE_FIRST_REMOTE_VIDEO_DECODED)wParam;
	BOOL bFound = FALSE;

	POSITION pos = m_listWndInfo.GetHeadPosition();
	while (pos != NULL)
	{
		AGVIDEO_WNDINFO &agvWndInfo = m_listWndInfo.GetNext(pos);
		if (agvWndInfo.nUID == lpData->uid)
		{
			bFound = TRUE;
			break;
		}
	}

	if (!bFound)
	{
		AGVIDEO_WNDINFO agvWndInfo;
		memset(&agvWndInfo, 0, sizeof(AGVIDEO_WNDINFO));
		agvWndInfo.nUID = lpData->uid;
		agvWndInfo.nWidth = lpData->width;
		agvWndInfo.nHeight = lpData->height;

		m_listWndInfo.AddTail(agvWndInfo);
	}

	RebindVideoWnd();
	delete lpData;

	if (!bFound)
	{
		AGVIDEO_WNDINFO & agvWndInfo = m_listWndInfo.GetTail();
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_JOINED lpData = (LPAGE_USER_JOINED)(wParam);
	EVStreamerParameter param;
	AfxGetEVLive()->GetParameter(param);
	
	if (lpData)
	{
		char buf[512] = { 0 };
		unsigned int	uid = lpData->uid;
		m_set_uid.insert(lpData->uid);
		int		elapsed = lpData->elapsed;
		if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
		{
			EVStreamerParameter evParam;
			AfxGetEVLive()->GetParameter(evParam);
			m_remote_layout.SetCanvasAttribute(evParam.videoResolutionWidth, evParam.videoResolutionHeight);

			m_remote_layout.UserJoinChannel(uid);
		}
		sprintf_s(buf, "uid %d, elapsed %d \n", uid, elapsed);
		OutputDebugStringA(buf);
		delete lpData;
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_OFFLINE lpData = (LPAGE_USER_OFFLINE)wParam;

	//其他用户离开，并且离开的用户是owner,则连麦观看者自动退出
	if (lpData->uid != m_uid && lpData->uid == m_ownerid)
	{
		OnLeaveChannel();
		m_staJoinChannelInfo.SetWindowText(_T("主播离开频道，连麦请求断开，请求连麦者退出频道"));
	}

	POSITION pos = m_listWndInfo.GetHeadPosition();
	while (pos != NULL)
	{
		if (m_listWndInfo.GetAt(pos).nUID == lpData->uid)
		{
			//远程用户离开，将其界面设为默认
			for (int i = 0; i < WNDVIDEO_COUNT; i++)
			{
				if (m_wndVideo[i].GetUID() == lpData->uid)
				{
					m_wndVideo[i].SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
					m_wndVideo[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
					break;
				}
			}
			m_listWndInfo.RemoveAt(pos);
			RebindVideoWnd();
			break;
		}

		m_listWndInfo.GetNext(pos);
	}



	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnRemoteVideoStat(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REMOTE_VIDEO_STAT lpData = (LPAGE_REMOTE_VIDEO_STAT)wParam;

	POSITION posNext = m_listWndInfo.GetHeadPosition();

	while (posNext != NULL)
	{
		AGVIDEO_WNDINFO &rWndInfo = m_listWndInfo.GetNext(posNext);

		if (rWndInfo.nUID == lpData->uid)
		{
			rWndInfo.nFramerate = lpData->receivedFrameRate;
			rWndInfo.nBitrate = lpData->receivedBitrate;
			rWndInfo.nWidth = lpData->width;
			rWndInfo.nHeight = lpData->height;
			m_wndVideo[rWndInfo.nIndex].SetFrameRateInfo(rWndInfo.nFramerate);
			m_wndVideo[rWndInfo.nIndex].SetBitrateInfo(rWndInfo.nBitrate);
			m_wndVideo[rWndInfo.nIndex].SetVideoResolution(rWndInfo.nWidth, rWndInfo.nHeight);
			break;
		}
	}

	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnAddToLive(WPARAM wParam, LPARAM lParam)
{
	USES_CONVERSION;
	CAGVideoWnd * videoWnd = (CAGVideoWnd *)wParam;
	int  index = (int)lParam;
	if (index < m_listWndInfo.GetCount())
	{
		WindowsInfo infoWindows;
		infoWindows.hWnd = m_wndVideo[index].GetSafeHwnd();
		TCHAR szTitle[200];
		TCHAR szClass[200];
		::GetWindowText(infoWindows.hWnd, szTitle, sizeof(szTitle) / sizeof(TCHAR));
		::GetClassName(infoWindows.hWnd, szClass, sizeof(szClass) / sizeof(TCHAR));
		strcpy_s(infoWindows.strClass, T2A(szClass));
		strcpy_s(infoWindows.strTitle, T2A(szTitle));
		// GetParent()->PostMessage(WM_ADDWINDOW, (WPARAM)gameInfo);
		AfxGetEVLive()->AddScene(&infoWindows);

		TCHAR* sourceName = szTitle;

		POSITION pos = m_listWndInfo.GetHeadPosition();
		int i = 0;
		while (pos != NULL)
		{
			if (index == i) // (m_listWndInfo.GetAt(pos).nUID == lpData->uid) 
			{
				if (sourceName)
				{
					wcscpy_s(m_listWndInfo.GetAt(pos).strLiveWndSourceName, sourceName);
				}
				else
				{
					memset(m_listWndInfo.GetAt(pos).strLiveWndSourceName, 0, sizeof(m_listWndInfo.GetAt(pos).strLiveWndSourceName));
				}
				break;
			}
			m_listWndInfo.GetNext(pos);
			++i;
		}
	}
	return 0;
}

//创建频道成功
LRESULT CChatUserDlg::OnEIDCreateChannel(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam);
	if (bSuccess)
	{
		LPEV_CREATE_CHANNEL_RES lpData = (LPEV_CREATE_CHANNEL_RES)lParam;
		m_channel_id = lpData->channel_id;
		m_push_url   = lpData->push_url;
		CString strGetUrl = _A2T(m_push_url.c_str()).c_str();
		strGetUrl.Replace(_T("wspush"), _T("wsrtmp"));
		//if (m_push_url)
		if (m_chkConfigurePublisher.GetCheck())
			m_edtPushUrlToOther.SetWindowText(strGetUrl.GetBuffer());
		CString strChannelId = _A2T(m_channel_id.c_str()).c_str();
		m_editChatRoom.SetWindowText(strChannelId);
		//主播创建成功，进行鉴权
		m_staJoinChannelInfo.SetWindowText(_T("创建频道成功，获取鉴权信息"));
		AfxGetEVLive()->UserAuth(app_id.c_str(), m_channel_id.c_str(), 0);
		delete lpData;
		lpData = NULL;
	}
	else
	{
		CString strError = (TCHAR*)lParam;
		AfxMessageBox(strError);
	}
	return 0;
}

//获取鉴权key
LRESULT CChatUserDlg::OnEIDUserAuth(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam);
	if (bSuccess)
	{
		m_staJoinChannelInfo.SetWindowText(_T("用户获取鉴权信息成功"));
		//主播获取鉴权key成功，设置角色并进行鉴权
		LPEV_USER_AUTH_RES lpData = (LPEV_USER_AUTH_RES)lParam;
		//保存owner_id
		AfxGetEVLive()->SetChannelProfile(EV_CHANNEL_PROFILE_LIVE_BROADCASTING);
		m_ownerid = lpData->owner_id;
		m_uid = lpData->uid;
		m_strChannelkey = lpData->channel_key;
		if (0 == AfxGetEVLive()->SetClientRole(EV_CLIENT_ROLE_BROADCASTER, lpData->auth_key)) //鉴权成功 
		{
			
			_EVJoinChannel(m_channel_id);
		}

		if (lpData)
		{
			delete lpData;
			lpData = NULL;
		}
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDEVJoinChannel(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam);
	if (bSuccess)
	{
		m_staJoinChannelInfo.SetWindowText(_T("向服务器发送进入频道的通知成功"));
		AfxGetEVLive()->StartCommHeart(app_id.c_str(), m_channel_id.c_str(), m_uid);
		//AfxMessageBox(_T("JoinEvChannel success"));
	}
	else
	{
		AfxMessageBox(_T("JoinEvChannel fail"));
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDBadNetWork(WPARAM wParam, LPARAM lParam)
{
	m_nReceiveBadNetCount++;

	if (m_nReceiveBadNetCount == MAX_RECV_BADNET_COUNT) //连续MAX_RECV_BADNET_COUNT次收到网络太差说明确实太差了，要提示用户
	{
		if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
			AfxMessageBox(_T("网络太差，主播退出频道"));
		else
			AfxMessageBox(_T("网络太差，连麦者退出频道"));
	}
	
	return 0;
}


LRESULT CChatUserDlg::OnEIDEVLeaveChannel(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam);
	if (bSuccess)
	{
		AfxMessageBox(_T("LeaveEVChannel success"));
	}
	else
	{
		AfxMessageBox(_T("LeaveEVChannel fail"));
	}
	return 0;
}

//三行两列
void CChatUserDlg::AdjustSizeVideo(int cx, int cy)
{
	int distance = 10; //窗口间距
	int iWidth  = (m_rcVideoArea.Width() - (cols + 1)*distance) / (cols);
	int iHeight = iWidth * VIDEO_RATIO;
	
	for (int i = 0; i < WNDVIDEO_COUNT; i++)
	{

		//int y = m_rcVideoArea.top + (m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * i);
		int x = ((i%cols) + 1)*distance + (i%cols)*iWidth;
		int y = ((i / cols) + 1)* distance + (i / cols)*iHeight;
		m_wndVideo[i].MoveWindow(x, y, iWidth, iHeight);
	}

	iWidth = iHeight / VIDEO_RATIO;
	//m_wndLocal.MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * WNDVIDEO_COUNT, iWidth, iHeight, FALSE);
}

void CChatUserDlg::InitCtrls()
{
	CRect ClientRect;


	GetClientRect(&ClientRect);

	CString str;

	for (int nIndex = 0; nIndex < 31; nIndex++)
	{
		m_cbxVideoProfile.InsertString(nIndex, m_szProfileDes[nIndex]);
		m_cbxVideoProfile.SetItemHeight(nIndex, 25);
		m_cbxVideoProfile.SetItemData(nIndex, (DWORD_PTR)m_nProfileValue[nIndex]);
	}



}

void CChatUserDlg::InitVideoWnd()
{
	 m_strWndClass = AfxRegisterWndClass(CS_DBLCLKS | CS_VREDRAW | CS_HREDRAW);

	 for (int nIndex = 0; nIndex < WNDVIDEO_COUNT; nIndex++)
	{
		CString strTitle;
		strTitle.Format(_T("render_%d"), nIndex);
		m_wndVideo[nIndex].Create(NULL, strTitle, WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + nIndex); // 
		m_wndVideo[nIndex].SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
		m_wndVideo[nIndex].SetFaceColor(RGB(0x58, 0x58, 0x58));
		m_wndVideo[nIndex].SetWndIndex(nIndex);
		m_wndVideo[nIndex].ShowVideoInfo(TRUE);
	}
// 
// 	m_wndLocal.Create(NULL, _T("render_self"), WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + 4); // WS_CHILD | 
// 	m_wndLocal.SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
// 	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
// 	m_wndLocal.SetUID(0);
// 	m_wndLocal.ShowVideoInfo(TRUE);

	//初始化角色身份 0 连麦观众 1 主播 2 观看者（PC端暂不支持，目前没有观看功能）
	int i = 0;
	m_cmbRole.InsertString(i++, _T("连麦观众"));
	m_cmbRole.InsertString(i++, _T("主播"));
	m_cmbRole.SendMessage(EM_SETREADONLY, true, 0);

	//m_editChatRoom.EnableWindow(FALSE);
}

void CChatUserDlg::SetDefaultBkg()
{
	for (int nIndex = 0; nIndex < WNDVIDEO_COUNT; nIndex++)
	{
		if (m_wndVideo[nIndex])
		{
			m_wndVideo[nIndex].SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
			m_wndVideo[nIndex].SetFaceColor(RGB(0x58, 0x58, 0x58));
		}
	}

// 	m_wndLocal.SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
// 	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
}

void CChatUserDlg::InitData()
{
	m_szProfileDes[0] = _T("160x120 15fps 65kbps");
	m_nProfileValue[0] = 0;
	m_szProfileDes[1] = _T("120x120 15fps 50kbps");
	m_nProfileValue[1] = 2;

	m_szProfileDes[2] = _T("320x180 15fps 140kbps");
	m_nProfileValue[2] = 10;
	m_szProfileDes[3] = _T("180x180 15fps 100kbps");
	m_nProfileValue[3] = 12;
	m_szProfileDes[4] = _T("240x180 15fps 120kbps");
	m_nProfileValue[4] = 13;

	m_szProfileDes[5] = _T("320x240 15fps 200kbps");
	m_nProfileValue[5] = 20;
	m_szProfileDes[6] = _T("240x240 15fps 140kbps");
	m_nProfileValue[6] = 22;
	m_szProfileDes[7] = _T("424x240 15fps 220kbps");
	m_nProfileValue[7] = 23;

	m_szProfileDes[8] = _T("640x360 15fps 400kbps");
	m_nProfileValue[8] = 30;
	m_szProfileDes[9] = _T("360x360 15fps 260kbps");
	m_nProfileValue[9] = 32;
	m_szProfileDes[10] = _T("640x360 30fps 600kbps");
	m_nProfileValue[10] = 33;
	m_szProfileDes[11] = _T("360x360 30fps 400kbps");
	m_nProfileValue[11] = 35;
	m_szProfileDes[12] = _T("480x360 15fps 320kbps");
	m_nProfileValue[12] = 36;
	m_szProfileDes[13] = _T("480x360 30fps 490kbps");
	m_nProfileValue[13] = 37;
	m_szProfileDes[14] = _T("640x360 15fps 800kbps");
	m_nProfileValue[14] = 38;

	m_szProfileDes[15] = _T("640x480 15fps 500kbps");
	m_nProfileValue[15] = 40;
	m_szProfileDes[16] = _T("480x480 15fps 400kbps");
	m_nProfileValue[16] = 42;
	m_szProfileDes[17] = _T("640x480 30fps 750kbps");
	m_nProfileValue[17] = 43;
	m_szProfileDes[18] = _T("480x480 30fps 600kbps");
	m_nProfileValue[18] = 44;
	m_szProfileDes[19] = _T("848x480 15fps 610kbps");
	m_nProfileValue[19] = 47;
	m_szProfileDes[20] = _T("848x480 30fps 930kbps");
	m_nProfileValue[20] = 48;

	m_szProfileDes[21] = _T("1280x720 15fps 1130kbps");
	m_nProfileValue[21] = 50;
	m_szProfileDes[22] = _T("1280x720 30fps 1710kbps");
	m_nProfileValue[22] = 52;
	m_szProfileDes[23] = _T("960x720 15fps 910kbps");
	m_nProfileValue[23] = 54;
	m_szProfileDes[24] = _T("960x720 30fps 1380kbps");
	m_nProfileValue[24] = 55;

	m_szProfileDes[25] = _T("1920x1080 15fps 2080kbps");
	m_nProfileValue[25] = 60;
	m_szProfileDes[26] = _T("1920x1080 30fps 3150kbps");
	m_nProfileValue[26] = 62;
	m_szProfileDes[27] = _T("1920x1080 60fps 4780kbps");
	m_nProfileValue[27] = 64;
	m_szProfileDes[28] = _T("2560x1440 30fps 4850kbps");
	m_nProfileValue[28] = 66;
	m_szProfileDes[29] = _T("3560x1440 60fps 7350kbps");
	m_nProfileValue[29] = 67;

	m_szProfileDes[30] = _T("3840x2160 30fps 8190kbps");
	m_nProfileValue[30] = 70;
	m_szProfileDes[31] = _T("3840x2160 60fps 13500kbps");
	m_nProfileValue[31] = 72;
}

void CChatUserDlg::ShowVideo()
{
	int distance = 10; //窗口间距
	int iWidth = (m_rcVideoArea.Width() - (cols + 1)*distance) / (cols);
	int iHeight = iWidth * VIDEO_RATIO;

	for (int i = 0; i < WNDVIDEO_COUNT; i++)
	{
		m_wndVideo[i].ShowWindow(SW_SHOW);
		m_wndVideo[i].SetBigShowFlag(FALSE);
		m_wndVideo[i].SetParent(this);
		AfxGetEVLive()->SetRemoteStreamType(m_wndVideo[i].GetUID(), EV_REMOTE_VIDEO_STREAM_HIGH);
		//int y = m_rcVideoArea.top + (m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * i);
		int x = ((i%cols) + 1)*distance + (i%cols)*iWidth;
		int y = ((i / cols) + 1)* distance + (i / cols)*iHeight;
		m_wndVideo[i].MoveWindow(x, y, iWidth, iHeight);		
	}
}

void CChatUserDlg::RebindVideoWnd()
{
	if (m_wndVideo[0].GetSafeHwnd() == NULL /*|| m_wndLocal.GetSafeHwnd() == NULL*/)
	{
		return;
	}

	EVVideoCanvas canvas;
	canvas.renderMode = EV_RENDER_MODE_ADAPTIVE;

	POSITION pos = m_listWndInfo.GetHeadPosition();
	for (int nIndex = 0; nIndex < WNDVIDEO_COUNT; nIndex++)
	{
		if (pos != NULL)
		{
			AGVIDEO_WNDINFO &agvWndInfo = m_listWndInfo.GetNext(pos);
			canvas.uid = agvWndInfo.nUID;
			canvas.view = m_wndVideo[nIndex].GetSafeHwnd();
			agvWndInfo.nIndex = nIndex;

			AfxGetEVLive()->SetupRemoteVideo(canvas);
			m_wndVideo[nIndex].SetUID(canvas.uid);
			m_wndVideo[nIndex].SetVideoResolution(agvWndInfo.nWidth, agvWndInfo.nHeight);
			m_wndVideo[nIndex].SetFrameRateInfo(agvWndInfo.nFramerate);
			m_wndVideo[nIndex].SetBitrateInfo(agvWndInfo.nBitrate);
		}
		else
		{
			m_wndVideo[nIndex].SetUID(0);
		}
	}

	if (m_listWndInfo.GetCount() >= 1 && m_listWndInfo.GetCount() < WNDVIDEO_COUNT)
	{
		ShowVideo();
	}
}

BOOL CChatUserDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 窗口标题一定要设置此数值,否则无法查找内部视频子窗口;
	SetWindowText(EV_VIDEO_CLASS);

	//AfxGetEVLive()->SetLogFilePath(NULL);
	AfxGetEVLive()->EnableLastmileTest(true);
	//AfxGetEVLive()->SetClientRole(EV_CLIENT_ROLE_BROADCASTER, NULL);

	InitVideoWnd();

	g_ChatUserWnd = m_hWnd;

	return TRUE;
}

void CChatUserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_rcVideoArea);

	m_rcVideoArea.bottom -= 92;//72;

	// 2人， 右上角子画面区域
	m_rcChildVideoArea.top = m_rcVideoArea.top + 10;
	m_rcChildVideoArea.bottom = m_rcChildVideoArea.top + 144;
	m_rcChildVideoArea.right = m_rcVideoArea.right - 14;
	m_rcChildVideoArea.left = m_rcChildVideoArea.right - 192;

	if (/*m_wndLocal.GetSafeHwnd() == NULL || */m_wndVideo[0].GetSafeHwnd() == NULL)
	{
		return;
	}

	AdjustSizeVideo(cx, cy);

	Invalidate(TRUE);
}


void CChatUserDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	// TODO:  在此处添加消息处理程序代码
	//if (bShow && GetSafeHwnd() != NULL) 
	//{
	//	RebindVideoWnd();
	//	int nSel = CAgoraObject::GetAgoraObject()->GetClientRole();
	//	// m_cbxRole.SetCurSel(nSel);
	//}
}

void CChatUserDlg::_EVJoinChannel(std::string channel_id/*, CString push_url*/)
{
	AfxGetEVLive()->EnableVideo(true);
	AfxGetEVLive()->EnableDauleStream(false);
	AfxGetEVLive()->SetVideoProfile(EV_VIDEO_PROFILE_360P, true);
	EVVideoCanvas vc;
	vc.uid = 0;
	//vc.view = m_wndLocal.GetSafeHwnd();
	vc.renderMode = EV_RENDER_MODE_TYPE::EV_RENDER_MODE_HIDDEN;
	AfxGetEVLive()->SetupLocalVideo(vc);
	AfxGetEVLive()->StartPreview();

	AfxGetEVLive()->StartScreenCapture(m_captureWnd);
	
	if (1 == m_chkConfigurePublisher.GetCheck() && m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)// 0:未选中  1:选中
	{
		EVPublisherConfiguration configurePublisher;
		EVStreamerParameter evParam;
		AfxGetEVLive()->GetParameter(evParam);
		configurePublisher.bitrate = evParam.maxVideoBitrate;
		configurePublisher.framerate = evParam.frameRate;
		configurePublisher.width = evParam.videoResolutionWidth;
		configurePublisher.height = evParam.videoResolutionHeight;
		configurePublisher.owner = true;
		configurePublisher.publishUrl = m_push_url.c_str();
		AfxGetEVLive()->ConfigurePublisher(configurePublisher);
	}


	
// #ifdef SERVER_PUSH
// 	// 使用服务器推流机制,客户端不必再次直播推流;
// 	//"{\"streamName\":\"rtmp://xxx\", \"owner\":true}";
// 	JsonValueEx jsTemp;
// 	jsTemp["streamName"] = "rtmp://wspush.easyvaas.com/record/sunchaotest";
// 	jsTemp["owner"] = "true";
// 	std::string strInfo = jsTemp.toStyledString();
// 	AfxGetEVLive()->JoinChannel(CT2CA(channel_id), strInfo.c_str());
// #else
	m_staJoinChannelInfo.SetWindowText(_T("正在进入频道"));
	int ret = AfxGetEVLive()->JoinChannel(channel_id.c_str(), NULL, m_uid, m_strChannelkey.c_str());
	if (ret == -2)
	{
		AfxMessageBox(_T("ERR_INVALID_ARGUMENT "));
	}
	else if (ret == -3)
	{
		AfxMessageBox(_T("ERR_NOT_READY "));
	}
	else if (ret == -5)
	{
		AfxMessageBox(_T("ERR_REFUSED  "));
	}

//#endif
}


// void CChatUserDlg::_OnAudienceJoinChannel(CString channel_id) //请求连麦者
// {
// 
// }
// 
// void CChatUserDlg::_OnBroadcasterJoinChannel(CString channel_id)
// {
// 
// 
// 	//_EVJoinChannel(channel_id);
// }


void CChatUserDlg::_CreateChannel(CString channel_id)
{
	//TString app_id = AflGetEVData()->GetAppID();
	std::string channel = _T2A(channel_id.GetBuffer(0));
	m_staJoinChannelInfo.SetWindowText(_T("开始创建频道"));
	AfxGetEVLive()->CreateChannel(const_cast<char*>(app_id.c_str()), const_cast<char*>(channel.c_str()));
}

void CChatUserDlg::OnJoinChannel()
{
	if (m_bJoinChannel)
	{
		m_staJoinChannelInfo.SetWindowText(_T("若想进入其他频道，请先离开当前频道"));
		return;
	}

	int cur_sel = m_cmbRole.GetCurSel();
	if (cur_sel < 0)
	{
		AfxMessageBox(_T("请选择身份"));
		return;
	}

	CString channel_id;
	m_editChatRoom.GetWindowText(channel_id);

	m_channel_id = _T2A(channel_id.GetBuffer(0));

	if (cur_sel == EVLIVE_USER_ROLE_BROADCASTER) //主播
		_CreateChannel(channel_id);//_OnBroadcasterJoinChannel(channel_id);
	else //请求连麦者
	{	
		if (channel_id.IsEmpty())
		{
			AfxMessageBox(_T("请在编辑框输入频道id,如果该id不存在则无法进入"));
			return;
		}
		AfxGetEVLive()->UserAuth(app_id.c_str(), _T2A(channel_id.GetBuffer(0)).c_str(), 0);
		//_CreateChannel(channel_id);
	}

	//_OnOwnerJoinChannel();
	//_EVJoinChannel(strChannelName);
}

void CChatUserDlg::OnLeaveChannel()
{	
	//
	if (!m_bJoinChannel)
	{
		m_staJoinChannelInfo.SetWindowText(_T("还未加入任何频道！"));
		return;
	}
	// 取消本地预览;
	AfxGetEVLive()->StopPreview();
	AfxGetEVLive()->LeaveChannel();
	//清理远程布局内部信息
	m_remote_layout.Clear();
	m_staJoinChannelInfo.SetWindowText(_T("离开频道"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 回调处理;

void CEVDemoCallBack::onJoinChannelSuccess(const char* channel, unsigned int uid, int elapsed)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = new AGE_JOINCHANNEL_SUCCESS;
	
	int nChannelLen = strlen(channel) + 1;
	lpData->channel = new char[nChannelLen];
	lpData->uid = uid;
	lpData->elapsed = elapsed;
	memset(lpData->channel, 0, 0);
	strcpy_s(lpData->channel, nChannelLen, channel);
	
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onRejoinChannelSuccess(const char* channel, unsigned int uid, int elapsed)
{
	LPAGE_REJOINCHANNEL_SUCCESS lpData = new AGE_REJOINCHANNEL_SUCCESS;

	int nChannelLen = strlen(channel) + 1;
	lpData->channel = new char[nChannelLen];
	lpData->uid = uid;
	lpData->elapsed = elapsed;
	strcpy_s(lpData->channel, nChannelLen, channel);

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_REJOINCHANNEL_SUCCESS), (WPARAM)lpData, 0);
}


void CEVDemoCallBack::onWarning(int warn, const char* msg)
{
	CString str;
	str = _T("onWarning");
	if (warn == 104)
		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_BAD_NETWORK), 0, 0);
}

void CEVDemoCallBack::onError(int err, const char* msg)
{
	LPAGE_ERROR lpData = new AGE_ERROR;

	int nMsgLen = 0;

	// attention: the pointer of msg maybe NULL!!!
	if (msg != NULL)
	{
		nMsgLen = strlen(msg) + 1;
		lpData->msg = new char[nMsgLen];
		strcpy_s(lpData->msg, nMsgLen, msg);
	}
	else
	{
		lpData->msg = NULL;
	}
	lpData->err = err;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_ERROR), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onAudioQuality(unsigned int uid, int quality, unsigned short delay, unsigned short lost)
{
	LPAGE_AUDIO_QUALITY lpData = new AGE_AUDIO_QUALITY;

	lpData->uid = uid;
	lpData->quality = quality;
	lpData->delay = delay;
	lpData->lost = lost;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_AUDIO_QUALITY), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onAudioVolumeIndication(const EVAudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume)
{
	LPAGE_AUDIO_VOLUME_INDICATION lpData = new AGE_AUDIO_VOLUME_INDICATION;

	lpData->speakers = new EVAudioVolumeInfo[speakerNumber];
	memcpy(lpData->speakers, speakers, speakerNumber*sizeof(EVAudioVolumeInfo));
	lpData->speakerNumber = speakerNumber;
	lpData->totalVolume = totalVolume;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_AUDIO_VOLUME_INDICATION), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onLeaveChannel(const EVRtcStats& stat)
{

	LPAGE_LEAVE_CHANNEL lpData = new AGE_LEAVE_CHANNEL;

	memcpy(&lpData->rtcStat, &stat, sizeof(EVRtcStats));

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onRtcStats(const EVRtcStats& stat)
{
	CString str;

	str = _T("stat");
}

void CEVDemoCallBack::onMediaEngineEvent(int evt)
{
	LPAGE_MEDIA_ENGINE_EVENT lpData = new AGE_MEDIA_ENGINE_EVENT;

	lpData->evt = evt;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_MEDIA_ENGINE_EVENT), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	LPAGE_AUDIO_DEVICE_STATE_CHANGED lpData = new AGE_AUDIO_DEVICE_STATE_CHANGED;

	int nDeviceIDLen = strlen(deviceId) + 1;

	lpData->deviceId = new char[nDeviceIDLen];

	strcpy_s(lpData->deviceId, nDeviceIDLen, deviceId);
	lpData->deviceType = deviceType;
	lpData->deviceState = deviceState;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_AUDIO_DEVICE_STATE_CHANGED), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState)
{
	LPAGE_VIDEO_DEVICE_STATE_CHANGED lpData = new AGE_VIDEO_DEVICE_STATE_CHANGED;

	int nDeviceIDLen = strlen(deviceId) + 1;

	lpData->deviceId = new char[nDeviceIDLen];

	strcpy_s(lpData->deviceId, nDeviceIDLen, deviceId);
	lpData->deviceType = deviceType;
	lpData->deviceState = deviceState;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_VIDEO_DEVICE_STATE_CHANGED), (WPARAM)lpData, 0);

}

void CEVDemoCallBack::onLastmileQuality(int quality)
{
	LPAGE_LASTMILE_QUALITY lpData = new AGE_LASTMILE_QUALITY;

	lpData->quality = quality;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_LASTMILE_QUALITY), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onFirstLocalVideoFrame(int width, int height, int elapsed)
{
	LPAGE_FIRST_LOCAL_VIDEO_FRAME lpData = new AGE_FIRST_LOCAL_VIDEO_FRAME;

	lpData->width = width;
	lpData->height = height;
	lpData->elapsed = elapsed;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onFirstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed)
{
	LPAGE_FIRST_REMOTE_VIDEO_DECODED lpData = new AGE_FIRST_REMOTE_VIDEO_DECODED;

	lpData->uid = uid;
	lpData->width = width;
	lpData->height = height;
	lpData->elapsed = elapsed;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onFirstRemoteVideoFrame(unsigned int uid, int width, int height, int elapsed)
{
	LPAGE_FIRST_REMOTE_VIDEO_FRAME lpData = new AGE_FIRST_REMOTE_VIDEO_FRAME;

	lpData->uid = uid;
	lpData->width = width;
	lpData->height = height;
	lpData->elapsed = elapsed;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_FIRST_REMOTE_VIDEO_FRAME), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onUserJoined(unsigned int uid, int elapsed)
{
	LPAGE_USER_JOINED lpData = new AGE_USER_JOINED;

	lpData->uid = uid;
	lpData->elapsed = elapsed;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_USER_JOINED), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onUserOffline(unsigned int uid, EV_USER_OFFLINE_REASON_TYPE reason)
{
	LPAGE_USER_OFFLINE lpData = new AGE_USER_OFFLINE;

	lpData->uid = uid;
	lpData->reason = reason;
	
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_USER_OFFLINE), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onUserMuteAudio(unsigned int uid, bool muted)
{
	LPAGE_USER_MUTE_AUDIO lpData = new AGE_USER_MUTE_AUDIO;

	lpData->uid = uid;
	lpData->muted = muted;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_USER_MUTE_AUDIO), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onUserMuteVideo(unsigned int uid, bool muted)
{
	LPAGE_USER_MUTE_VIDEO lpData = new AGE_USER_MUTE_VIDEO;

	lpData->uid = uid;
	lpData->muted = muted;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_USER_MUTE_VIDEO), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onApiCallExecuted(const char* api, int error)
{
	LPAGE_APICALL_EXECUTED lpData = new AGE_APICALL_EXECUTED;

	strcpy_s(lpData->api, 128, api);
	lpData->error = error;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_APICALL_EXECUTED), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onLocalVideoStats(const EVLocalVideoStats& stats)
{
	LPAGE_LOCAL_VIDEO_STAT lpData = new AGE_LOCAL_VIDEO_STAT;

	lpData->sentBitrate = stats.sentBitrate;
	lpData->sentFrameRate = stats.sentFrameRate;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_LOCAL_VIDEO_STAT), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onRemoteVideoStats(const EVRemoteVideoStats& stats)
{
	LPAGE_REMOTE_VIDEO_STAT lpData = new AGE_REMOTE_VIDEO_STAT;

	lpData->uid = stats.uid;
	lpData->delay = stats.delay;
	lpData->width = stats.width;
	lpData->height = stats.height;
	lpData->receivedFrameRate = stats.receivedFrameRate;
	lpData->receivedBitrate = stats.receivedBitrate;
	lpData->receivedFrameRate = stats.receivedFrameRate;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_REMOTE_VIDEO_STAT), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onCreateChannel(IN const bool& bSuccessed, IN const char* strErrInfo, IN const char* channel_id, IN const char* push_url)
{
	
	if (bSuccessed)
	{
		LPEV_CREATE_CHANNEL_RES lpData = new EV_CREATE_CHANNEL_RES;

		memset(lpData->channel_id, 0, sizeof(lpData->channel_id));
		memset(lpData->push_url, 0, sizeof(lpData->push_url));

		int len = strlen(channel_id);
		memcpy_s(lpData->channel_id, sizeof(lpData->channel_id), channel_id, len);
		len = strlen(push_url);
		memcpy_s(lpData->push_url, sizeof(lpData->channel_id), push_url, len);
		//strcpy_s(lpData->push_url, strlen(lpData->push_url), push_url);
		//memcpy_s((bufptr)+(offset), (maxsize)-(offset), (src), (count))
		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_CREATE_CHANNEL), true, (WPARAM)lpData);
	}
	else
	{
	//	AfxMessageBox(_T("CreateChannel失败"));
		
		std::string sError = strErrInfo;
		std::wstring wsError =_A2T(sError.c_str());
		AfxMessageBox(wsError.c_str());

		//	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_CREATE_CHANNEL), false, (WPARAM)sError.c_str());
	}
}

void CEVDemoCallBack::onUserAuth(IN const bool& bSuccessed, IN const char* strErrInfo, IN const char* auth_key, IN const unsigned int owner_id, const char* channel_key, const unsigned int uid)
{
	if (bSuccessed)
	{
		LPEV_USER_AUTH_RES lpData = new EV_USER_AUTH_RES;


		memset(lpData->auth_key, 0, sizeof(lpData->auth_key));
		int len = strlen(auth_key);
		memcpy_s(lpData->auth_key, sizeof(lpData->auth_key), auth_key, len);

		memset(lpData->channel_key, 0, sizeof(lpData->channel_key));
		len = strlen(channel_key);
		memcpy_s(lpData->channel_key, sizeof(lpData->channel_key), channel_key, len);

		lpData->owner_id = owner_id;
		lpData->uid = uid;

		//strcpy_s(lpData->auth_key, sizeof(lpData->auth_key), auth_key);
		//strcpy_s(lpData->owner_id, strlen(lpData->owner_id), owner_id);
		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_USER_AUTH), true, (WPARAM)lpData);
	}
	else
	{
		//	AfxMessageBox(_T("CreateChannel失败"));
		CString strError;
		strError.Format(_T("%s"), strErrInfo);
		MessageBoxA(NULL, strErrInfo, NULL, MB_OK);
		//::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_USER_AUTH), false, (WPARAM)strError.GetString());
	}
}
void CEVDemoCallBack::onLeaveEVChannel(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	if (bSuccessed)
	{
		
	}
	else
	{
		MessageBoxA(NULL, strErrInfo, NULL, MB_OK);
	}
}

void CEVDemoCallBack::onJoinEVChannel(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	if (bSuccessed)
	{
		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_USER_JOIN), bSuccessed, 0);
	}
	else
	{
		MessageBoxA(NULL, strErrInfo, NULL, MB_OK);
	}
}

void CEVDemoCallBack::onStartCommHeart(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	if (bSuccessed)
	{

	}
	else
	{
		MessageBoxA(NULL, strErrInfo, NULL, MB_OK);
	}
}

//////////////////////////////////////////////////////////////////////
void CEVDemoCallBack::onCameraReady()
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_CAMERA_READY), 0, 0);
}

void CEVDemoCallBack::onVideoStopped()
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_VIDEO_STOPPED), 0, 0);
}

void CEVDemoCallBack::onConnectionLost()
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_CONNECTION_LOST), 0, 0);
}

void CEVDemoCallBack::onConnectionInterrupted()
{
	CString str;

	str = _T("onConnectionInterrupted");
}

void CEVDemoCallBack::onUserEnableVideo(unsigned int uid, bool enabled)
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_CONNECTION_LOST), 0, 0);
}

void CEVDemoCallBack::onStartRecordingService(int error)
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_START_RCDSRV), 0, 0);
}

void CEVDemoCallBack::onStopRecordingService(int error)
{
	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_STOP_RCDSRV), 0, 0);
}

void CEVDemoCallBack::onRefreshRecordingServiceStatus(int status)
{
	LPAGE_RCDSRV_STATUS lpData = new AGE_RCDSRV_STATUS;

	lpData->status = status;

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_REFREASH_RCDSRV), (WPARAM)lpData, 0);
}

void CChatUserDlg::OnSelchangeComboRole()
{
	int cur_sel = m_cmbRole.GetCurSel();
	m_user_choose_role = (EVLive_User_Role)cur_sel;

	if (m_user_choose_role == EVLIVE_USER_ROLE_AUDIENCE)
	{
		m_chkConfigurePublisher.EnableWindow(false);
	}
	else if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		m_chkConfigurePublisher.EnableWindow(true);
	}
}



void CChatUserDlg::OnBnClickedCheckConfigurepublisher()
{
	// TODO: Add your control notification handler code here
}
