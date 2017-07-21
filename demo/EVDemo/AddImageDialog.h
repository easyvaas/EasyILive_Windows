#pragma once
#include "afxcmn.h"

// Í¼Æ¬Ô´´°¿Ú;
class CAddImageDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddImageDialog)

public:
	CAddImageDialog(SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddImageDialog();

	enum { IDD = IDD_ADDIMAGEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

	void InitData();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedImageOpenButton();

	virtual void OnOK();
	virtual void OnCancel();

private:
	CEdit				m_edtImagePath;
	SceneStatus			m_staScene;
	CSliderCtrl			m_ctrlBkg;
};
