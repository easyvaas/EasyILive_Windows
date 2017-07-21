#include "stdafx.h"
#include "ChatUserDlg.h"
#include "AGEventDef.h"
#include "EVDemoCallBack.h"
#include "JsonValueEx.h"

#define IDC_BASEWND_VIDEO               113
#define	VIDEO_RATIO						(9.0 / 16.0)		// width / height
	
HWND g_ChatUserWnd = NULL;

IMPLEMENT_DYNAMIC(CChatUserDlg, CDialogEx)

CChatUserDlg::CChatUserDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(CChatUserDlg::IDD, pParent)
{
}

CChatUserDlg::~CChatUserDlg()
{
}

void CChatUserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CHANNEL, m_editChatRoom);
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

	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_JOIN_CHANNEL, &CChatUserDlg::OnJoinChannel)
	ON_BN_CLICKED(IDC_BUTTON_LEAVE_CHANNEL, &CChatUserDlg::OnLeaveChannel)
END_MESSAGE_MAP()


// CChatUserDlg 消息处理程序


LRESULT CChatUserDlg::OnJoinChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_JOINCHANNEL_SUCCESS lpData = (LPAGE_JOINCHANNEL_SUCCESS)wParam;
	m_listWndInfo.RemoveAll();

	delete lpData;
	return 0;
}

LRESULT CChatUserDlg::OnLeaveChannel_CallBack(WPARAM wParam, LPARAM lParam)
{
	LPAGE_REJOINCHANNEL_SUCCESS lpData = (LPAGE_REJOINCHANNEL_SUCCESS)wParam;

	m_listWndInfo.RemoveAll();

	// 离开后将背景图改成默认;
	SetDefaultBkg();

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
		int		elapsed = lpData->elapsed;
		sprintf_s(buf, "uid %d, elapsed %d \n", uid, elapsed);
		OutputDebugStringA(buf);
		delete lpData;
	}
	return 0;
}

LRESULT CChatUserDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam)
{
	LPAGE_USER_OFFLINE lpData = (LPAGE_USER_OFFLINE)wParam;

	POSITION pos = m_listWndInfo.GetHeadPosition();
	while (pos != NULL)
	{
		if (m_listWndInfo.GetAt(pos).nUID == lpData->uid)
		{
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

void CChatUserDlg::AdjustSizeVideo(int cx, int cy)
{
	int iHeight = m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) - 10;
	int iWidth = iHeight / VIDEO_RATIO;

	for (int i = 0; i < WNDVIDEO_COUNT; i++)
	{
		m_wndVideo[i].MoveWindow(0, m_rcVideoArea.top + (m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * i), iWidth, iHeight, FALSE);
	}

	iWidth = iHeight / VIDEO_RATIO;
	m_wndLocal.MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * WNDVIDEO_COUNT, iWidth, iHeight, FALSE);
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

	m_wndLocal.Create(NULL, _T("render_self"), WS_CHILD | WS_VISIBLE | WS_BORDER, CRect(0, 0, 1, 1), this, IDC_BASEWND_VIDEO + 4); // WS_CHILD | 
	m_wndLocal.SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
	m_wndLocal.SetUID(0);
	m_wndLocal.ShowVideoInfo(TRUE);
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

	m_wndLocal.SetBackImage(IDB_BACKGROUND_VIDEO, 96, 96, RGB(0x44, 0x44, 0x44));
	m_wndLocal.SetFaceColor(RGB(0x58, 0x58, 0x58));
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
	m_wndLocal.ShowWindow(SW_HIDE);
	m_wndLocal.SetBigShowFlag(FALSE);

	int iHeight = m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) - 10;
	int iWidth = iHeight / VIDEO_RATIO;

	for (int nIndex = 0; nIndex < WNDVIDEO_COUNT; nIndex++) 
	{
		m_wndVideo[nIndex].ShowWindow(SW_SHOW);
		m_wndVideo[nIndex].SetBigShowFlag(FALSE);
		m_wndVideo[nIndex].SetParent(this);

		AfxGetEVLive()->SetRemoteStreamType(m_wndVideo[nIndex].GetUID(), EV_REMOTE_VIDEO_STREAM_HIGH);

		m_wndVideo[nIndex].MoveWindow(0, m_rcVideoArea.top + (m_rcVideoArea.Height() / 5 * nIndex), iWidth, iHeight, FALSE);
	}

	// 最后一个默认为本地显示;
	CRect	rcLocal;
	int		nRemoteCount = m_listWndInfo.GetCount();
	m_wndLocal.MoveWindow(0, m_rcVideoArea.top + m_rcVideoArea.Height() / (WNDVIDEO_COUNT + 1) * WNDVIDEO_COUNT, iWidth, iHeight, FALSE);

	m_wndLocal.SetParent(this);
	m_wndLocal.ShowWindow(SW_SHOW);
}

void CChatUserDlg::RebindVideoWnd()
{
	if (m_wndVideo[0].GetSafeHwnd() == NULL || m_wndLocal.GetSafeHwnd() == NULL)
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

	if (m_listWndInfo.GetCount() > 1 && m_listWndInfo.GetCount() < WNDVIDEO_COUNT)
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
	AfxGetEVLive()->SetClientRole(EV_CLIENT_ROLE_BROADCASTER, NULL);

	InitVideoWnd();

	g_ChatUserWnd = m_hWnd;

	return TRUE;
}

void CChatUserDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	GetClientRect(&m_rcVideoArea);

	m_rcVideoArea.bottom -= 72;

	// 2人， 右上角子画面区域
	m_rcChildVideoArea.top = m_rcVideoArea.top + 10;
	m_rcChildVideoArea.bottom = m_rcChildVideoArea.top + 144;
	m_rcChildVideoArea.right = m_rcVideoArea.right - 14;
	m_rcChildVideoArea.left = m_rcChildVideoArea.right - 192;

	if (m_wndLocal.GetSafeHwnd() == NULL || m_wndVideo[0].GetSafeHwnd() == NULL)
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


void CChatUserDlg::OnJoinChannel()
{
	CString strChannelName;
	m_editChatRoom.GetWindowText(strChannelName);
	if (strChannelName.IsEmpty())
	{
		return;
	}

	AfxGetEVLive()->SetChannelProfile(EV_CHANNEL_PROFILE_LIVE_BROADCASTING);
	AfxGetEVLive()->SetVideoProfile(EV_VIDEO_PROFILE_360P, true);
	AfxGetEVLive()->EnableVideo(true);
	AfxGetEVLive()->EnableDauleStream(false);

	EVVideoCanvas vc;
	vc.uid = 0;
	vc.view = m_wndLocal.GetSafeHwnd();
	vc.renderMode = EV_RENDER_MODE_TYPE::EV_RENDER_MODE_HIDDEN;
	AfxGetEVLive()->SetupLocalVideo(vc);
	AfxGetEVLive()->StartPreview();

	AfxGetEVLive()->StartScreenCapture(m_captureWnd);

#ifdef SERVER_PUSH
	// 使用服务器推流机制,客户端不必再次直播推流;
	//"{\"streamName\":\"rtmp://xxx\", \"owner\":true}";
	JsonValueEx jsTemp;
	jsTemp["streamName"] = "rtmp://wspush.easyvaas.com/record/sunchaotest";
	jsTemp["owner"] = "true";
	std::string strInfo = jsTemp.toStyledString();
	AfxGetEVLive()->JoinChannel(CT2CA(strChannelName), strInfo.c_str());
#else
	AfxGetEVLive()->JoinChannel(CT2CA(strChannelName), NULL);
#endif
}

void CChatUserDlg::OnLeaveChannel()
{	
	// 取消本地预览;
	AfxGetEVLive()->StopPreview();

	AfxGetEVLive()->LeaveChannel();
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