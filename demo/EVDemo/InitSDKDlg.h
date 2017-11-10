#pragma once
#include "Resource.h"

// CInitSDKDlg 对话框

class CInitSDKDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CInitSDKDlg)

public:
	CInitSDKDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInitSDKDlg();

	// 对话框数据
	enum { IDD = IDD_INITSDK_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedInitBtn();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};