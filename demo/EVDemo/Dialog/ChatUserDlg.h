#pragma once

#include "resource.h"
#include "afxwin.h"
#include "resource.h"
#include "AGVideoWnd.h"
#include <list>
#include <vector>
#include "EVLiveLock.h"
#include <set>

// CChatUserDlg 对话框

#define WNDVIDEO_COUNT 6	// 目前最多支持7路(包括自己，即该数值最大为6);

class CCompositionLayout 
{
public:
	CCompositionLayout();
	~CCompositionLayout();

	void UserJoinChannel(unsigned int uid);
	void UserLeaveChannel(unsigned int uid);
	void Clear();
	void SetCanvasAttribute(int width, int height, const char* bgColor = NULL, const char* appData = NULL, int appDataLen = 0);
private:
	void _SetCompositionLayout();
private:
	CLiveLockEx m_lock;
	std::vector<unsigned int> m_vec_uid;
	//std::vector<>
	//维护布局的变化
	EVVideoCompositingLayout m_layout;

	int m_width;
	int m_height;
	//3ha
	const int cols = 2;
	const int rows = 3;
	const int max_cols = cols + 2;
	const int max_rows = rows;
	EVVideoCompositingLayout::Region regions[7];//最多7个
};

class CChatUserDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CChatUserDlg)

public:
	CChatUserDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CChatUserDlg();

	// 对话框数据
	enum { IDD = IDD_CHATUSER_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
private:
	void InitCtrls();
	void InitData();
	void InitVideoWnd();

	void ShowVideo();

	// 设置默认背景图;
	void SetDefaultBkg();

	void AdjustSizeVideo(int cx, int cy);

	HWND GetRemoteVideoWnd(int nIndex);
//	HWND GetLocalVideoWnd() { return m_wndLocal.GetSafeHwnd(); };

	void RebindVideoWnd();

	void _EVJoinChannel(std::string channel_id); // 频道id  旁路推流地址(主播不需要设置旁路推流地址)
// 	void _OnAudienceJoinChannel(CString channel_id); //请求连麦
// 	void _OnBroadcasterJoinChannel(CString channel_id);
	//主播需要创建
	void _CreateChannel(CString channel_id);


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
//	CAGVideoWnd		m_wndLocal;
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

	afx_msg LRESULT OnAddToLive(WPARAM wParam, LPARAM lParam);

	EV_CLIENT_ROLE_TYPE m_client_role;
	EVLive_User_Role m_user_choose_role;
	std::string m_channel_id;//通过编辑框输入获取，主播调用CreateChannel接口可能返回channel_id。
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

	DWORD   m_nReceiveBadNetCount = 0; //接收网络太差104警告次数
	const   DWORD   MAX_RECV_BADNET_COUNT = 5;
public:

	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

	afx_msg void OnJoinChannel();
	afx_msg void OnLeaveChannel();

	// 用于处理引擎的回调消息;
	afx_msg LRESULT OnJoinChannel_CallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLeaveChannel_CallBack(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReJoinChannel_CallBack(WPARAM wParam, LPARAM lParam);

	afx_msg LRESULT OnEIDFirstLocalFrame(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDFirstRemoteFrameDecoded(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDConnectionLost(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDVideoDeviceChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRemoteVideoStat(WPARAM wParam, LPARAM lParam);
	
	afx_msg LRESULT OnJoin(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLeave(WPARAM wParam, LPARAM lParam);
	CComboBox m_cmbRole;
	afx_msg void OnSelchangeComboRole();


	afx_msg LRESULT OnEIDCreateChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDUserAuth(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDEVJoinChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDEVLeaveChannel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnEIDBadNetWork(WPARAM wParam, LPARAM lParam);

	
	
	CButton m_chkConfigurePublisher;
	afx_msg void OnBnClickedCheckConfigurepublisher();
	CEdit m_edtPushUrlToOther;
	CStatic m_staJoinChannelInfo;
	CButton m_btnJoinChannel;
	CButton m_btnLeaveChannel;
};
