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
	//ά�����ֵı仯
	EVVideoCompositingLayout m_layout;

	int m_width;
	int m_height;
	//3ha
	const int cols = 2;
	const int rows = 3;
	const int max_cols = cols + 2;
	const int max_rows = rows;
	EVVideoCompositingLayout::Region regions[7];//���7��
};

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
// 	void _OnAudienceJoinChannel(CString channel_id); //��������
// 	void _OnBroadcasterJoinChannel(CString channel_id);
	//������Ҫ����
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
