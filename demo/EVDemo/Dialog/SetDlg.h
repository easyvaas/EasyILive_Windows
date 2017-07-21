#pragma once
#include "afxwin.h"
#include "Resource.h"


// CSetDlg 对话框

class CSetDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CSetDlg)

public:
	CSetDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetDlg();

// 对话框数据
	enum { IDD = IDD_SET_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_cmbRate;
	CComboBox m_cmbFPS;
	CComboBox m_cmbMaxBit;
	afx_msg void OnBnClickedOk();
	CEdit m_edtFps;
	CEdit m_edtMaxbit;
};
