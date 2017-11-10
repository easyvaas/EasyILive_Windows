#include "stdafx.h"
#include "Demo.h"
#include "AddWindowDialog.h"
#include "ChatUserDlg.h"
#include "AGEventDef.h"
// CAddWindowDialog 对话框
IMPLEMENT_DYNAMIC(CAddWindowDialog, CDialogEx)

CAddWindowDialog::CAddWindowDialog(GameInfo* info, int info_count, SceneStatus status/* = SCENE_ADD*/, CWnd* pParent /*=NULL*/)
: CDialogEx(CAddWindowDialog::IDD, pParent)
{
	m_staScene = status;
	//m_pWindowOpts = new GameInfo[MAX_GAMEOPTS_COUNT];
	m_iOptsCount = info_count;
	m_pWindowOpts = info;
}

CAddWindowDialog::~CAddWindowDialog()
{
	m_pWindowOpts = NULL;
// 	delete[]m_pWindowOpts;
// 	m_pWindowOpts = NULL;
}

void CAddWindowDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_WINDOW_OPTIONS_COMBO, m_cmbWindowOpts);
}


BEGIN_MESSAGE_MAP(CAddWindowDialog, CDialogEx)
	ON_BN_CLICKED(IDC_WINDOW_UPDATE_BUTTON, &CAddWindowDialog::OnBnClickedGameUpdateButton)
END_MESSAGE_MAP()


BOOL CAddWindowDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AddWindowOpts();

	if (SCENE_MODIFY == m_staScene)
	{
		InitData();
	}

	return TRUE;
}

void CAddWindowDialog::OnBnClickedGameUpdateButton()
{
	RemoveAllGameOpts();
	AddWindowOpts();
}

void CAddWindowDialog::RemoveAllGameOpts()
{
	for (int i = m_cmbWindowOpts.GetCount() - 1; i >= 0; i--)
	{
		m_cmbWindowOpts.DeleteString(i);
	}
	m_cmbWindowOpts.SetCurSel(-1);
	m_iOptsCount = 0;
}

void CAddWindowDialog::AddWindowOpts()
{
	//AfxGetEVLive()->GetGameOptions(m_pWindowOpts, m_iOptsCount);

	// 先删除再添加;
	for (int i = 0; i < m_iOptsCount; i++)
	{
		CString strClass = CA2CT(m_pWindowOpts[i].strClass);
		CString strTitle = CA2CT(m_pWindowOpts[i].strTitle);
		CString strItemText;
		strItemText.Format(_T("[%s]: %s"), strClass, strTitle);
		m_cmbWindowOpts.InsertString(i, strItemText);
	}
}

void CAddWindowDialog::InitData()
{
	SetDlgItemText(IDOK, _T("确定"));
}

void CAddWindowDialog::OnOK()
{
	int iSel = m_cmbWindowOpts.GetCurSel();
	if ((iSel < 0) || (iSel >= m_cmbWindowOpts.GetCount()))
	{
		MessageBox(_T("请选择窗口！"));
		return;
	}

	WindowsInfo infoWindows(m_pWindowOpts[iSel]);
	cursel = iSel;
	switch (m_staScene)
	{

	default:
		break;
	}

	CDialogEx::OnOK();
}

void CAddWindowDialog::OnCancel()
{
	CDialogEx::OnCancel();
}
