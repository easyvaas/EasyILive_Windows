#pragma once

// 游戏源;
class CAddWindowDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddWindowDialog)

public:
	CAddWindowDialog(GameInfo* info, int info_count, SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddWindowDialog();

	// 对话框数据;
	enum { IDD = IDD_ADDWINDOWDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedGameUpdateButton();
	virtual void OnOK();
	virtual void OnCancel();
	int cursel = -1;
protected:
	void AddWindowOpts();
	void RemoveAllGameOpts();
	void InitData();

private:
	CComboBox		m_cmbWindowOpts;
	SceneStatus		m_staScene;

	// 游戏选项;
	GameInfo*		m_pWindowOpts;
	int				m_iOptsCount;
};
