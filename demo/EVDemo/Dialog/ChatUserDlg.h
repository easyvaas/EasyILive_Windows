#pragma once

#include "resource.h"
#include "afxwin.h"
#include "resource.h"
#include "AGVideoWnd.h"
#include <list>
#include <vector>
#include "EVLiveLock.h"
#include <set>
// CChatUserDlg �Ի���

#define WNDVIDEO_COUNT 6	// Ŀǰ���֧��7·(�����Լ���������ֵ���Ϊ6);


class CCompositionLayout 
{
public:
	CCompositionLayout();
	~CCompositionLayout();

	void UserJoinChannel(unsigned int uid, bool bBroadCast=false);
	void UserLeaveChannel(unsigned int uid);
	void BroadCastLeaveChannel(unsigned int uid);
	void ChangeLayout(unsigned int uid);
	void SetCanvasAttribute(SIZE szCanvasLayout , const char* bgColor = NULL, const char* appData = NULL, int appDataLen = 0);
private:
	void _SetCompositionLayout();
private:
	CLiveLockEx m_lock;
	std::vector<unsigned int> m_vec_uid;
	std::set<unsigned int> m_set_uid;

	//std::vector<>
	//ά�����ֵı仯
	EVVideoCompositingLayout m_layout;

	int m_width;
	int m_height;
    
	int m_realWidth;
	int m_realHeight;
	//3ha
	const int cols = 2;
	const int rows = 3;
	const int max_cols = cols + 2;
	const int max_rows = rows;
	EVVideoCompositingLayout::Region regions[7];//���7��
};

///////////////////////////////////////////////////////////////////

class CChatUserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatUserDlg)

public:
	CChatUserDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CChatUserDlg();

	// �Ի�������
	enum { IDD = IDD_CHATUSER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:

	enum AGORA_FRAME_TYPE
	{
		MAX_AGORA_FRAME_720 = 0,
		MAX_AGORA_FRAME_360,
		MAX_AGORA_FRAME_180
	};
	void InitCtrls();
	void InitData();
	void InitVideoWnd();

	void ShowVideo();

	// ����Ĭ�ϱ���ͼ;
	void SetDefaultBkg();

	void AdjustSizeVideo(int cx, int cy);

	HWND GetRemoteVideoWnd(int nIndex);
//	HWND GetLocalVideoWnd() { return m_wndLocal.GetSafeHwnd(); };

	void RebindVideoWnd();

	void _EVJoinChannel(std::string channel_id); // Ƶ��id  ��·������ַ(��������Ҫ������·������ַ)

	static DWORD WINAPI Thread_StartCommHeart(LPVOID lpvoid);
private:
	CComboBox m_cbxVideoProfile;
	int                 m_nProfileValue[64];
	LPTSTR			    m_szProfileDes[64];

	std::string         app_id;
public:
	HWND		m_captureWnd;
	CString		m_strWndClass;

private:
	CEdit m_editChatRoom;
	const int LocalWndWidth = 160;
	const int LocalWndheight = 90;

	CAGVideoWnd		m_wndLocal;
	CAGVideoWnd		m_wndVideo[WNDVIDEO_COUNT];
	CRect			m_rcVideoArea;
	CRect			m_rcChildVideoArea;

	// UINT         m_bindingUID;

	typedef struct _AGVIDEO_WNDINFO
	{
		UINT	nUID;
		int		nIndex;

		UINT	nWidth;
		UINT	nHeight;
		int		nBitrate;
		int		nFramerate;
		int		nCodec;

		TCHAR strLiveWndSourceName[256];

	} AGVIDEO_WNDINFO, *PAGVIDEO_WNDINFO, *LPAGVIDEO_WNDINFO;

	CList<AGVIDEO_WNDINFO>	m_listWndInfo;

	//afx_msg LRESULT OnAddToLive(WPARAM wParam, LPARAM lParam);

	EV_CLIENT_ROLE_TYPE m_client_role;
	EVLive_User_Role m_user_choose_role;
	std::string m_channel_id;//ͨ���༭�������ȡ����������CreateChannel�ӿڿ��ܷ���channel_id��
	std::string m_push_url;

	std::string m_strChannelkey;
	unsigned int m_uid;
	unsigned int m_ownerid; // owener_id
	
	std::set<unsigned int> m_set_uid;
	CCompositionLayout m_remote_layout;

	const int rows = 3;
	const int cols = 2;

	bool  m_bFirstJoinChannel = true;
	bool  m_bJoinChannel = false;

	DWORD   m_nReceiveBadNetCount = 0; //��������̫��104�������
	const   DWORD   MAX_RECV_BADNET_COUNT = 5;
	AGORA_FRAME_TYPE     m_nCommType = MAX_AGORA_FRAME_720;

	HANDLE m_hMonitorThread = NULL;

	static unsigned int WINAPI MonitorShareWindow(LPVOID lpVoid);

	std::string m_canvasBgCaolor = "#FF0000";
public:

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg void OnJoinChannel();
	afx_msg void OnLeaveChannel();

	// ���ڴ�������Ļص���Ϣ;
	afx_msg LRESULT OnJoinChannel_CallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLeaveChannel_CallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReJoinChannel_CallBack(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnEIDFirstLocalFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDFirstRemoteFrameDecoded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserMuteVideo(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDConnectionLost(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDVideoDeviceChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoteVideoStat(WPARAM wParam, LPARAM lParam);
	
	afx_msg LRESULT OnJoin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLeave(WPARAM wParam, LPARAM lParam);
	CComboBox m_cmbRole;
	afx_msg void OnSelchangeComboRole();

	afx_msg LRESULT OnEIDBadNetWork(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDCommHeartBeat(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDStartShare(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDStartScreenCapture(WPARAM wParam, LPARAM lParam);//����
	afx_msg LRESULT OnEIDStartLocalPreview(WPARAM wParam, LPARAM lParam);//����
	
	afx_msg LRESULT OnEIDShareWindowChanged(WPARAM wParam, LPARAM lParam);//����

	afx_msg LRESULT OnEIDChangedVideoProfile(WPARAM wParam, LPARAM lParam);
	CButton m_chkConfigurePublisher;

	CEdit m_edtPushUrlToOther;
	CStatic m_staJoinChannelInfo;
	CButton m_btnJoinChannel;
	CButton m_btnLeaveChannel;
	
	afx_msg void OnClickedRadio720();
	afx_msg void OnRadio360();
	afx_msg void OnRadio180();
	CButton m_radio720;

	HWND m_hWndTestWnd = NULL;
	CEdit m_edtUID;
	afx_msg void OnBnClickedButtonStopShare();

	RECT m_rcDesktop;
	SIZE m_szDeskTop;

	SIZE m_szMaxRemoteLayoutResolution;//��·���������������ֱ���
	afx_msg void OnBnClickedButtonEnableVideo();
	afx_msg void OnBnClickedButtonMuteVideo();
	
};
extern HWND g_ChatUserWnd;
extern HWND g_ShareWnd;