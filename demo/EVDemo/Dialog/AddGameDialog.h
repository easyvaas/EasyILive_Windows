#pragma once

// 游戏源;
class CAddGameDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddGameDialog)

public:
	CAddGameDialog(SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddGameDialog();

	// 对话框数据;
	enum { IDD = IDD_ADDGAMEDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedGameUpdateButton();
	virtual void OnOK();
	virtual void OnCancel();

protected:
	void AddGameOpts();
	void RemoveAllGameOpts();
	void InitData();

private:
	CComboBox		m_cmbGameOpts;
	SceneStatus		m_staScene;

	// 游戏选项;
	GameInfo*		m_pGameOpts;
	int				m_iOptsCount;
};
