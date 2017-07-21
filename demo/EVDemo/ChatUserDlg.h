#pragma once

#include "resource.h"
#include "afxwin.h"
#include "resource.h"
#include "AGVideoWnd.h"

// CChatUserDlg 对话框

#define WNDVIDEO_COUNT 4	// 目前最多支持7路(包括自己，即该数值最大为6);

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
	HWND GetLocalVideoWnd() { return m_wndLocal.GetSafeHwnd(); };

	void RebindVideoWnd();

private:
	CComboBox m_cbxVideoProfile;
	int                 m_nProfileValue[64];
	LPTSTR			    m_szProfileDes[64];

public:
	HWND		m_captureWnd;
	CString		m_strWndClass;

private:
	CEdit m_editChatRoom;
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

	afx_msg LRESULT OnAddToLive(WPARAM wParam, LPARAM lParam);

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
};
