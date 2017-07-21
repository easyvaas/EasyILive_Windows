#pragma once
#include "afxwin.h"
#include "afxcolorbutton.h"
#include "afxcmn.h"

// 文本源;
class CAddTextDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddTextDialog)

public:
	CAddTextDialog(SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddTextDialog();

	enum { IDD = IDD_ADDTEXTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	virtual BOOL OnInitDialog();

	void InitData();

	void DisplayOutline();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedTextOutlineCheck();

	virtual void OnOK();
	virtual void OnCancel();

private:
	CEdit				m_edtContent;
	CComboBox			m_cmbFontName;
	CMFCColorButton		m_btnFontColor;
	CButton				m_btnFontBold;
	CButton				m_btnFontItalic;
	CButton				m_btnUnderLine;
	CButton				m_btnTextVer;
	CSliderCtrl			m_ctrSpeed;

	CButton m_btnOutLineUsed;
	CMFCColorButton		m_btnOutLineColor;
	CEdit				m_edtOutLineThick;
	SceneStatus			m_staScene;
};
