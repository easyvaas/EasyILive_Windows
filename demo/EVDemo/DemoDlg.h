#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "Dialog/ChatUserDlg.h"

// CDemoDlg 对话框
class CDemoDlg : public CDialogEx
{
// 构造
public:
	CDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	~CDemoDlg();
// 对话框数据
	enum { IDD = IDD_DEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:

    afx_msg void OnBnClickedScreenshotButton();
    afx_msg void OnBnClickedCameraButton();
	afx_msg void OnBnClickedWindowsButton();

	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


private:
	CSliderCtrl		m_ctrMicrop;
	CSliderCtrl		m_ctrDestop;

	CChatUserDlg	m_dlgChatUser;
	GameInfo*		m_pWindowOpts;
	int m_iOptsCount;
public:
	
	


	
};
