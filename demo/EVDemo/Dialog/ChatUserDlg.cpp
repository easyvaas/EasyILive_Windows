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
HWND g_ShareWnd = NULL;
RECT rcShareWnd;
CCompositionLayout::CCompositionLayout()
{
	m_vec_uid.clear();
}

CCompositionLayout::~CCompositionLayout()
{

}

void CCompositionLayout::SetCanvasAttribute(SIZE szCanvasLayout, const char* bgColor, const char* appData, int appDataLen)
{
	m_layout.canvasWidth     = szCanvasLayout.cx;    //窗口宽度
	m_layout.canvasHeight    = szCanvasLayout.cy;   //窗口高度
	m_layout.backgroundColor = bgColor;
	m_layout.appData         = appData;
	m_layout.appDataLength   = appDataLen;
	m_width = szCanvasLayout.cx;
	m_height = szCanvasLayout.cy;

	if (NULL != g_ShareWnd)
	{
		RECT rc = { 0 };
		::GetWindowRect(g_ShareWnd, &rc);
		m_realWidth = rc.right - rc.left;
		m_realHeight = rc.bottom - rc.top;
	}
	else
	{
		m_realWidth = m_width; 
		m_realHeight = m_height;
	}
}

void CCompositionLayout::UserJoinChannel(unsigned int uid, bool bBroadCast)
{
	m_lock.Lock();
	if (m_set_uid.find(uid) == m_set_uid.end())//没找到
	{
		//第一个保存主播uid，如果出去了再重新进来
		m_set_uid.insert(uid);
		m_vec_uid.push_back(uid);

		if (bBroadCast)
		{
			for (int i = m_vec_uid.size() - 1; i > 0;--i)
			{
				m_vec_uid[i] = m_vec_uid[i - 1];
			}
			m_vec_uid[0] = uid;
		}
		
	}
	//ChangeLayout(uid);
	_SetCompositionLayout();
	m_lock.UnLock();
}

void CCompositionLayout::UserLeaveChannel(unsigned int uid)
{
	m_lock.Lock();
// 	
// 	//只剩主播自己
// 	if (m_vec_uid.size() == 1)
// 	{
// 		_SetCompositionLayout();
// 	}

	for (auto iter = m_vec_uid.begin(); iter != m_vec_uid.end(); ++iter)
	{
		if (uid == *iter)
		{
			m_vec_uid.erase(iter);
			auto set_iter = m_set_uid.find(uid);
			m_set_uid.erase(set_iter);
			break;
		}
	}
	
	if (m_vec_uid.size() > 0)
	{
		_SetCompositionLayout();
	}
	m_lock.UnLock();
}

void CCompositionLayout::BroadCastLeaveChannel(unsigned int uid)
{
	for (int i = 0; i < 7; i++)
	{
		regions[i].width  = 0;
		regions[i].height = 0;
		regions[i].x = 0;
		regions[i].y = 0;
		regions[i].zOrder = 0;
		regions[i].alpha = 0;
		regions[i].uid = 0;
	}
	m_layout.regions = regions;
	AfxGetEVLive()->SetVideoCompositingLayout(m_layout);
}

void CCompositionLayout::ChangeLayout(unsigned int uid)
{
	int find_index = -1;
	m_lock.Lock();
	for (int i = 0; i < m_vec_uid.size(); ++i)
	{
		if (uid == m_vec_uid[i])
		{
			find_index = i;
			break;
		}
	}
	m_lock.UnLock();

	m_layout.regionCount = m_vec_uid.size();
	SIZE szChange;
	EV_LIVE_TYPE type = AfxGetEVLive()->GetEVLiveType();
	//共享窗口和截屏窗口大小会变化
	if (type == EV_LIVE_TYPE_SHAREWINDOW)
	{
		RECT rc = { 0 };
		::GetWindowRect(g_ShareWnd, &rc);
		szChange = { rc.right - rc.left, rc.bottom - rc.top };
	}
	else if(EV_LIVE_TYPE_SCREEN_CAPTURE == type)//EV_LIVE_TYPE_SCREEN_CAPTURE
	{
		RECT rcScreenCapture;
		AfxGetEVLive()->GetRealComositionLayoutRect(rcScreenCapture);
		szChange = { rcScreenCapture.right - rcScreenCapture.left, rcScreenCapture.bottom - rcScreenCapture.top };
	}
	else
	{
		RECT rcCamera;
		AfxGetEVLive()->GetRealComositionLayoutRect(rcCamera);
		float h = rcCamera.bottom - rcCamera.top;
		float w = rcCamera.right - rcCamera.left;
		//
		int temp_w = ((float)m_height / (float)h)*w;
		if (m_width <= ((float)m_height / (float)h)*w)
		{
			szChange.cx = m_width;
			szChange.cy = ((float)m_width / (float)w)*h;
		}
		else if (m_height < ((float)m_width / (float)w)*h)
		{
			szChange.cy = m_height;
			szChange.cx = ((float)m_height / (float)h)*w;
		}
	}

	//横纵间距都是20像素
	int distance = 20;
	double distance_rate = 20 / (double)m_height;

	double h = (m_height - (max_rows + 1) * 20) / (double)max_rows;

	//double w = (m_width - (max_cols + 1) * 20) / (double)max_cols;

	double rate_h = (double)h / m_height;
	double rate_w = rate_h;


	float offset_x = 0.0;
	float offset_y = 0.0;

	for (int i = 0; i < m_vec_uid.size(); ++i)
	{
		regions[i].uid = m_vec_uid[i];
		regions[i].alpha = 1.0;

		regions[i].renderMode = EV_RENDER_MODE_FIT;
		
		if (0 == i)
		{
			regions[i].width = (szChange.cx) / (double)m_width;
			regions[i].height = szChange.cy/ (double)m_height;
			regions[i].x = (m_width - szChange.cx) / (double)(2 * m_width);
			regions[i].y = (m_height - szChange.cy) / (double)(2 * m_height);
			regions[i].zOrder = 0;	
		}
		else
		{
			regions[i].width = rate_w;
			regions[i].height = rate_h;
			regions[i].x = 1.0 - ((i - 1) / rows + 1)*(distance_rate + rate_w);
			regions[i].y = ((i - 1) % rows + 1)*(distance_rate)+((i - 1) % rows)*rate_h;
			regions[i].zOrder = 1;

		}
	}
	m_layout.regions = regions;
	AfxGetEVLive()->SetVideoCompositingLayout(m_layout);
}

//三行两列，纵向占满全部高度，横向最多两列，所以先计算高度
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

	
	float offset_x = 0.0;
	float offset_y = 0.0;
	
	for (int i = 0; i < m_vec_uid.size(); ++i)
	{
		regions[i].uid         = m_vec_uid[i];
		regions[i].alpha       = 1.0;
	
		regions[i].renderMode  = EV_RENDER_MODE_FIT;
		if (0 == i)
		{
			regions[i].width = m_realWidth / m_width;//1.0;
			regions[i].height  = m_realHeight/m_height;
			regions[i].x = offset_x;
			regions[i].y = offset_y;
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
	}

// 	for (int i = m_vec_uid.size(); i < 7; ++i)
// 	{
// 		regions[i].width = 0;
// 		regions[i].height = 0;
// 		regions[i].x = 0;
// 		regions[i].y = 0;
// 		regions[i].zOrder = 0;
// 		regions[i].uid = 0;
// 		
// 	}
	m_layout.regions = regions;
	AfxGetEVLive()->SetVideoCompositingLayout(m_layout);
}



IMPLEMENT_DYNAMIC(CChatUserDlg, CDialogEx)

CChatUserDlg::CChatUserDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CChatUserDlg::IDD, pParent)
{

	app_id = "53957274";//theApp.app_id;//theApp.app_id;

	m_szMaxRemoteLayoutResolution.cx = 1920;
	m_szMaxRemoteLayoutResolution.cy = 1080;
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
	DDX_Control(pDX, IDC_RADIO_720, m_radio720);
	DDX_Control(pDX, IDC_EDIT_UID, m_edtUID);
}


BEGIN_MESSAGE_MAP(CChatUserDlg, CDialogEx)
	//ON_MESSAGE(WM_ADDTOLIVE, &CChatUserDlg::OnAddToLive)
	
	ON_MESSAGE(WM_MSGID(EID_JOINCHANNEL_SUCCESS), &CChatUserDlg::OnJoinChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), &CChatUserDlg::OnLeaveChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_REJOINCHANNEL_SUCCESS), &CChatUserDlg::OnReJoinChannel_CallBack)
	ON_MESSAGE(WM_MSGID(EID_FIRST_LOCAL_VIDEO_FRAME), &CChatUserDlg::OnEIDFirstLocalFrame)
	ON_MESSAGE(WM_MSGID(EID_FIRST_REMOTE_VIDEO_DECODED), &CChatUserDlg::OnEIDFirstRemoteFrameDecoded)
	ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CChatUserDlg::OnEIDUserOffline)
	ON_MESSAGE(WM_MSGID(EID_USER_MUTE_VIDEO), &CChatUserDlg::OnEIDUserMuteVideo)
	ON_MESSAGE(WM_MSGID(EID_REMOTE_VIDEO_STAT), &CChatUserDlg::OnRemoteVideoStat)
	ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CChatUserDlg::OnEIDUserJoined)

	ON_MESSAGE(WM_MSGID(EID_EV_BAD_NETWORK), &CChatUserDlg::OnEIDBadNetWork)
	ON_MESSAGE(WM_MSGID(EID_EV_COMM_HEART_BEAT), &CChatUserDlg::OnEIDCommHeartBeat)

	ON_MESSAGE(WM_MSGID(EID_WINDOWS_SHARE_START), &CChatUserDlg::OnEIDStartShare)
	ON_MESSAGE(WM_MSGID(EID_SCREEN_CAPTURE_DESKTOP), &CChatUserDlg::OnEIDStartScreenCapture)
	ON_MESSAGE(WM_MSGID(EID_START_LOCAL_PREVIEW), &CChatUserDlg::OnEIDStartLocalPreview)
	ON_MESSAGE(WM_MSGID(WM_MSG_SHAREWINDOW_SIZE), &CChatUserDlg::OnEIDShareWindowChanged)
	ON_MESSAGE(WM_MSGID(EID_CHANED_VIDEOPROFILE), &CChatUserDlg::OnEIDChangedVideoProfile)

	
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOIN_CHANNEL, &CChatUserDlg::OnJoinChannel)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &CChatUserDlg::OnLeaveChannel)
	ON_CBN_SELCHANGE(IDC_COMBO_ROLE, &CChatUserDlg::OnSelchangeComboRole)
	
	ON_BN_CLICKED(IDC_RADIO_720, &CChatUserDlg::OnClickedRadio720)
	ON_COMMAND(IDC_RADIO_360, &CChatUserDlg::OnRadio360)
	ON_COMMAND(IDC_RADIO_180, &CChatUserDlg::OnRadio180)
	ON_BN_CLICKED(IDC_BUTTON_STOP_SHARE, &CChatUserDlg::OnBnClickedButtonStopShare)
	ON_BN_CLICKED(IDC_BUTTON_ENABLE_VIDEO, &CChatUserDlg::OnBnClickedButtonEnableVideo)
	ON_BN_CLICKED(IDC_BUTTON_MUTE_VIDEO, &CChatUserDlg::OnBnClickedButtonMuteVideo)
	
END_MESSAGE_MAP()


// CChatUserDlg 消息处理程序


LRESULT CChatUserDlg::OnJoinChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;
	m_listWndInfo.RemoveAll();

	//m_uid = lpData->uid;
	m_set_uid.insert(lpData->uid);

	m_uid = lpData->uid;
	m_channel_id = lpData->channel;


	m_editChatRoom.SetWindowText(CA2T(lpData->channel));
	CString strUid; 
	strUid.Format(_T("%u"), lpData->uid);
	
	m_edtUID.SetWindowText(strUid);

	//m_edtPushUrlToOther.SetWindowText(strUrl);
	CString strUrl = CA2T(lpData->push_url);
	m_edtPushUrlToOther.SetWindowText(strUrl);
	if (m_user_choose_role == EVLIVE_USER_ROLE_AUDIENCE)//连麦请求者
		m_staJoinChannelInfo.SetWindowText(_T("连麦请求者进入频道成功!"));
	else
	{
		m_staJoinChannelInfo.SetWindowText(_T("主播成功进入频道!"));
	}

	m_bJoinChannel = true;
	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		//设置旁路推流比例
		m_remote_layout.SetCanvasAttribute(m_szMaxRemoteLayoutResolution);
		m_remote_layout.UserJoinChannel(lpData->uid, true);
		//m_remote_layout.ChangeLayout(m_uid);
	}

	delete[] lpData->push_url;
	lpData->push_url = NULL;
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
	if (iter != m_set_uid.end())
		m_set_uid.erase(iter);
	//离开频道
	//AfxGetEVLive()->LeaveEVChannel(app_id.c_str(), m_channel_id.c_str(), m_uid);
	// 离开后将背景图改成默认;
	SetDefaultBkg();
	AfxGetEVLive()->StopScreenCapture();
	m_edtPushUrlToOther.SetWindowText(_T(""));
	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		m_remote_layout.SetCanvasAttribute(m_szMaxRemoteLayoutResolution);
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
	
	if (lpData)
	{
		char buf[512] = { 0 };
		unsigned int	uid = lpData->uid;
		m_set_uid.insert(lpData->uid);
		int		elapsed = lpData->elapsed;
		if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
		{
			m_remote_layout.SetCanvasAttribute(m_szMaxRemoteLayoutResolution);
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
	//  owner如果一场离开，可能还会再次进入，所以一段时间之内，频道内的连麦者不用离开。
	//  在心跳返回的地方，其他用户自动离开


	// 	POSITION pos = m_listWndInfo.GetHeadPosition();
	// 	while (pos != NULL)
	// 	{
	// 		if (m_listWndInfo.GetAt(pos).nUID == lpData->uid)
	// 		{
	// 			//远程用户离开，将其界面设为默认
	// 			for (int i = 0; i < WNDVIDEO_COUNT; i++)
	// 			{
	// 				if (m_wndVideo[i].GetUID() == lpData->uid)
	// 				{
	// 					m_wndVideo[i].SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
	// 					m_wndVideo[i].SetFaceColor(RGB(0x58, 0x58, 0x58));
	// 					break;
	// 				}
	// 			}
	// 			m_listWndInfo.RemoveAt(pos);
	// 			RebindVideoWnd();
	// 			break;
	// 		}
	// 
	// 		m_listWndInfo.GetNext(pos);
	// 	}

	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		m_remote_layout.SetCanvasAttribute(m_szMaxRemoteLayoutResolution);
		m_remote_layout.UserLeaveChannel(lpData->uid);
	}

	delete lpData;

	return 0;
}

LRESULT CChatUserDlg::OnEIDUserMuteVideo(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_MUTE_VIDEO lpData = (LPAGE_USER_MUTE_VIDEO)wParam;
	//true,禁止视频画面
	if (lpData->muted)
	{
		if (lpData->uid == m_uid)//本地画面禁掉
		{
			AfxMessageBox(_T("本地画面禁掉"));
		}
		else
		{
			AfxMessageBox(_T("远端画面禁掉"));
			if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
				m_remote_layout.UserLeaveChannel(lpData->uid);
		}
	}
	else
	{
		if (lpData->uid == m_uid)//本地画面禁掉
		{
			AfxMessageBox(_T("本地画面打开"));
		}
		else
		{
			AfxMessageBox(_T("远端画面打开"));
			if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
			{
				m_remote_layout.UserJoinChannel(lpData->uid, m_uid == lpData->uid);
			}
		}
	}

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


LRESULT CChatUserDlg::OnEIDCommHeartBeat(WPARAM wParam, LPARAM lParam)
{
	bool bSuccess = static_cast<bool>(wParam); 
	if (bSuccess)
	{
		bool bOwnerOnline = static_cast<bool>(lParam);
		if (bOwnerOnline)
		{

		}
		else//主播不在线了连麦者请退出
		{
			if (m_user_choose_role == EVLIVE_USER_ROLE_AUDIENCE)
			{
			 	OnLeaveChannel();
			 	m_staJoinChannelInfo.SetWindowText(_T("主播退出频道，连麦请求断开，请求连麦者退出频道"));
			}
		}
	}
	else
	{
		
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDStartShare(WPARAM wParam, LPARAM lParam)
{
	if (AfxGetEVLive()->IsScreenCapture())
		AfxGetEVLive()->StopScreenCapture();
	AfxGetEVLive()->StartScreenCapture(g_ShareWnd);
	
	m_remote_layout.ChangeLayout(m_uid);
	return 0;
}

LRESULT CChatUserDlg::OnEIDStartScreenCapture(WPARAM wParam, LPARAM lParam)
{
	CRect* lpRect = (CRect*)lParam;
	
	RECT rectSel = { lpRect->left, lpRect->top, lpRect->right, lpRect->bottom };

	if (AfxGetEVLive()->IsScreenCapture())
		AfxGetEVLive()->StopScreenCapture();
	
	AfxGetEVLive()->StartScreenCapture(NULL, 15, &rectSel);
	AfxGetEVLive()->StartPreview();
	m_remote_layout.ChangeLayout(m_uid);
	return 0;
}

//预览
LRESULT CChatUserDlg::OnEIDStartLocalPreview(WPARAM wParam, LPARAM lParam)
{
	AfxGetEVLive()->SetVideoProfile((EV_VIDEO_PROFILE_TYPE)EV_VIDEO_PROFILE_480P, FALSE);
	AfxGetEVLive()->EnableVideo(true);
	AfxGetEVLive()->SetLocalPreviewMirror(false);
	//
	EVVideoCanvas vc;
	vc.uid = 0;
	vc.renderMode = EV_RENDER_MODE_FIT;
	vc.view = m_captureWnd;

	int ret = AfxGetEVLive()->SetupLocalVideo(vc);
 
  	if (AfxGetEVLive()->hasCamera())
  		AfxGetEVLive()->StartPreview();
	return 0;
}



//三行两列
void CChatUserDlg::AdjustSizeVideo(int cx, int cy)
{
	int distance = 5; //窗口间距
	int iWidth = 160;//(m_rcVideoArea.Width() - (cols + 1)*distance) / (cols);
	int iHeight = 90;//iWidth * VIDEO_RATIO;
	
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

	//初始化角色身份 0 连麦观众 1 主播 2 观看者（PC端暂不支持，目前没有观看功能）
	int i = 0;
	m_cmbRole.InsertString(i++, _T("连麦观众"));
	m_cmbRole.InsertString(i++, _T("主播"));
	m_cmbRole.SendMessage(EM_SETREADONLY, true, 0);
	
	m_cmbRole.SetCurSel(1);
	m_chkConfigurePublisher.SetCheck(true);
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
	int distance = 5; //窗口间距
	int iWidth = 160;//(m_rcVideoArea.Width() - (cols + 1)*distance) / (cols);
	int iHeight = 90;//iWidth * VIDEO_RATIO;

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
	canvas.renderMode = EV_RENDER_MODE_FIT;

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
			m_wndVideo[nIndex].SetVideoResolution(160/*agvWndInfo.nWidth*/, 90/*agvWndInfo.nHeight*/);
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

LRESULT CChatUserDlg::OnEIDShareWindowChanged(WPARAM wParam, LPARAM lParam)
{
	//保证其他连麦者看到的画面完整
	if (g_ShareWnd != NULL && ::IsWindow(g_ShareWnd))
	{
		AfxGetEVLive()->StartScreenCapture(g_ShareWnd);
	}

	//修改旁路推流布局，连麦请求者可以不用调用
	SIZE szChangeSize = { rcShareWnd.right - rcShareWnd.left, rcShareWnd.bottom - rcShareWnd.top };
	m_remote_layout.SetCanvasAttribute(m_szMaxRemoteLayoutResolution);
	m_remote_layout.ChangeLayout(m_uid);
	return 0;
}

LRESULT CChatUserDlg::OnEIDChangedVideoProfile(WPARAM wParam, LPARAM lParam)
{
	EVVideoCanvas vc;
	vc.uid = 0;
	vc.renderMode = EV_RENDER_MODE_FIT;
	vc.view = m_captureWnd;
	int ret = AfxGetEVLive()->SetupLocalVideo(vc);
	return 0;
}

unsigned int WINAPI CChatUserDlg::MonitorShareWindow(LPVOID lpVoid)
{
	while (true)
	{
		if (g_ShareWnd != NULL && ::IsWindow(g_ShareWnd))
		{
			RECT rcCurrent;
			::GetWindowRect(g_ShareWnd, &rcCurrent);
			if ((rcCurrent.bottom - rcCurrent.top != rcShareWnd.bottom - rcShareWnd.top)
				|| (rcCurrent.right - rcCurrent.left != rcShareWnd.right - rcShareWnd.left))
			{
				rcShareWnd = rcCurrent;
				::SendMessage(g_ChatUserWnd, WM_MSGID(WM_MSG_SHAREWINDOW_SIZE), 0, 0);
			}
			Sleep(500);
		}
	}
	
	return 0;
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
	m_radio720.SetCheck(true);
	m_nCommType = MAX_AGORA_FRAME_720;
	
	g_ChatUserWnd = m_hWnd;

	RECT rc;
	GetWindowRect(&rc);

	GetDesktopWindow()->GetWindowRect(&m_rcDesktop);
	m_szDeskTop.cx = m_rcDesktop.right - m_rcDesktop.left;
	m_szDeskTop.cy = m_rcDesktop.bottom - m_rcDesktop.top; 
	m_hMonitorThread =(HANDLE)_beginthreadex(NULL, 0, MonitorShareWindow, this, 0, NULL);
	SetThreadPriority(m_hMonitorThread, THREAD_PRIORITY_ABOVE_NORMAL);

	OnEIDStartLocalPreview(0, 0);
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
}

void CChatUserDlg::_EVJoinChannel(std::string channel_id/*, CString push_url*/)
{
	EV_VIDEO_PROFILE_TYPE type = EV_VIDEO_PROFILE_720P;
	int bitrate = 1130;
	int framerate = 15;
	int videoResolutionWidth  = 1280;
	int videoResolutionHeight = 720;
	if (m_nCommType == MAX_AGORA_FRAME_720)
	{
		type = EV_VIDEO_PROFILE_720P;//// 1280x720  15   1130
		bitrate = 1130;
		framerate = 15;
		videoResolutionWidth = 1280;
		videoResolutionHeight = 720;
	}
	else if (m_nCommType == MAX_AGORA_FRAME_360)
	{
		type = EV_VIDEO_PROFILE_360P;// 640x360   15   400
		bitrate = 400;
		framerate = 15;
		videoResolutionWidth = 640;
		videoResolutionHeight = 360;
	}
	else if (m_nCommType == MAX_AGORA_FRAME_180)
	{
		type = EV_VIDEO_PROFILE_180P;//// 320x180   15   140
		bitrate = 140;
		framerate = 15;
		videoResolutionWidth = 320;
		videoResolutionHeight = 180;
	}

	EVVideoCanvas vc;
	vc.uid = 0;
	vc.renderMode = EV_RENDER_MODE_FIT;
	vc.view = m_captureWnd;
	int ret = AfxGetEVLive()->SetupLocalVideo(vc);

	if (NULL != g_ShareWnd)
	{
		RECT rc = { 0 };
		::GetWindowRect(g_ShareWnd, &rc);
		videoResolutionWidth = rc.right - rc.left;
		videoResolutionHeight = rc.bottom - rc.top;
	}
//	AfxGetEVLive()->SetupLocalVideo(vc);
	m_staJoinChannelInfo.SetWindowText(_T("正在进入频道"));
	
	if (m_chkConfigurePublisher.GetCheck())
	{
		EVPublisherConfiguration config;
		config.bitrate = bitrate;
		config.framerate = framerate;
		config.height = videoResolutionHeight;
		config.width = videoResolutionWidth;
		AfxGetEVLive()->ConfigurePublisher(config);
		if (NULL != g_ShareWnd)
		{
			::GetWindowRect(g_ShareWnd, &rcShareWnd);
			AfxGetEVLive()->StartScreenCapture(g_ShareWnd);
		}
	}
	
	/*int*/ ret = AfxGetEVLive()->JoinChannel(app_id.c_str(), m_channel_id.c_str(), m_uid, m_user_choose_role);//最后两个参数，默认进行旁路推流和录播
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


void CChatUserDlg::OnJoinChannel()
{

	int cur_sel = m_cmbRole.GetCurSel();
	if (cur_sel < 0)
	{
		AfxMessageBox(_T("请选择身份"));
		return;
	}

	CString channel_id;
	m_editChatRoom.GetWindowText(channel_id);
	m_channel_id = _T2A(channel_id.GetBuffer(0));

	CString suid;
	m_edtUID.GetWindowText(suid);
	
	if (m_user_choose_role==EVLIVE_USER_ROLE_BROADCASTER && !channel_id.IsEmpty() && suid.IsEmpty())//
	{
		m_staJoinChannelInfo.SetWindowText(_T("频道id不为空，必须输入主播的uid"));
		return;
	}
	m_uid = _ttoi(suid.GetBuffer(0));
	m_user_choose_role = (EVLive_User_Role)cur_sel;
	_EVJoinChannel(m_channel_id);

}

void CChatUserDlg::OnLeaveChannel()
{	
	m_remote_layout.UserLeaveChannel(m_uid);//主播离开，设置旁路布局
	AfxGetEVLive()->LeaveChannel();
	
	//清理远程布局内部信息
//	m_remote_layout.Clear();
	m_staJoinChannelInfo.SetWindowText(_T("离开频道"));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
// 回调处理;

//连麦请求者不会获得push_url，因为只有主播创建频道的时候会获取旁路推流地址。
void CEVDemoCallBack::onJoinChannelSuccess(const char* channel, unsigned int uid, int elapsed, const char* push_url)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = new AGE_JOINCHANNEL_SUCCESS;
	
	int nChannelLen  = strlen(channel) + 1;
	int nUrllLen     = strlen(push_url) + 1;
	lpData->channel  = new char[nChannelLen];
	lpData->push_url = new char[strlen(push_url) + 1];
	lpData->uid      = uid;
	lpData->elapsed  = elapsed;

	memset(lpData->channel, 0, 0);
	strcpy_s(lpData->channel, nChannelLen, channel);
	
	memset(lpData->push_url, 0, 0);
	strcpy_s(lpData->push_url, nUrllLen, push_url);

	::PostMessage(g_ChatUserWnd, WM_MSGID(EID_JOINCHANNEL_SUCCESS), (WPARAM)lpData, 0);
}

void CEVDemoCallBack::onJoinChannelError(const char* Error)
{
	MessageBoxA(NULL, Error, NULL, MB_OK);
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

void CEVDemoCallBack::onLeaveChannel(bool bSuccess, const char* strErrInfo, const EVRtcStats& stat)
{

	if (bSuccess)
	{
		LPAGE_LEAVE_CHANNEL lpData = new AGE_LEAVE_CHANNEL;

		memcpy(&lpData->rtcStat, &stat, sizeof(EVRtcStats));

		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_LEAVE_CHANNEL), (WPARAM)lpData, 0);
	}
	else
	{
		MessageBoxA(NULL, strErrInfo, NULL, MB_OK);
	}
	
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

void CEVDemoCallBack::onStartCommHeart(IN const bool& bSuccessed, IN const char* strErrInfo, bool ownerOnline )
{
	if (bSuccessed)
	{
		::PostMessage(g_ChatUserWnd, WM_MSGID(EID_EV_COMM_HEART_BEAT), bSuccessed, ownerOnline);
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
		m_chkConfigurePublisher.SetCheck(false);
	}
	else if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)
	{
		m_chkConfigurePublisher.EnableWindow(true);
		m_chkConfigurePublisher.SetCheck(true);
	}
}



void CChatUserDlg::OnClickedRadio720()
{
	m_nCommType = MAX_AGORA_FRAME_720;
}


void CChatUserDlg::OnRadio360()
{
	m_nCommType = MAX_AGORA_FRAME_360;
}


void CChatUserDlg::OnRadio180()
{
	m_nCommType = MAX_AGORA_FRAME_180;
}


void CChatUserDlg::OnBnClickedButtonStopShare()
{
	AfxGetEVLive()->StopScreenCapture();
	AfxGetEVLive()->StopPreveiw();
	m_remote_layout.ChangeLayout(m_uid);
}


void CChatUserDlg::OnBnClickedButtonEnableVideo()
{
	m_remote_layout.UserJoinChannel(m_uid, true);
	AfxGetEVLive()->MuteLocalVideo(FALSE);
}


void CChatUserDlg::OnBnClickedButtonMuteVideo()
{
	if (m_user_choose_role == EVLIVE_USER_ROLE_BROADCASTER)//主播mute画面
	{
		m_remote_layout.UserLeaveChannel(m_uid);
	}
	AfxGetEVLive()->MuteLocalVideo();
}


