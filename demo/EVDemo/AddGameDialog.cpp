#include "stdafx.h"
#include "Demo.h"
#include "AddGameDialog.h"

// CAddGameDialog 对话框
IMPLEMENT_DYNAMIC(CAddGameDialog, CDialogEx)

CAddGameDialog::CAddGameDialog(SceneStatus status/* = SCENE_ADD*/, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddGameDialog::IDD, pParent)
{
	m_staScene = status;
	m_pGameOpts = new GameInfo[MAX_GAMEOPTS_COUNT];
	m_iOptsCount = 0;
}

CAddGameDialog::~CAddGameDialog()
{
	delete[]m_pGameOpts;
	m_pGameOpts = NULL;
}

void CAddGameDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GAME_OPTIONS_COMBO, m_cmbGameOpts);
}


BEGIN_MESSAGE_MAP(CAddGameDialog, CDialogEx)
	ON_BN_CLICKED(IDC_GAME_UPDATE_BUTTON, &CAddGameDialog::OnBnClickedGameUpdateButton)
END_MESSAGE_MAP()


BOOL CAddGameDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AddGameOpts();

	if (SCENE_MODIFY == m_staScene)
	{
		InitData();
	}

	return TRUE;
}

void CAddGameDialog::OnBnClickedGameUpdateButton()
{
	RemoveAllGameOpts();
	AddGameOpts();
}

void CAddGameDialog::RemoveAllGameOpts()
{
	for (int i = m_cmbGameOpts.GetCount() - 1; i >= 0; i--)
	{
		m_cmbGameOpts.DeleteString(i);
	}
	m_cmbGameOpts.SetCurSel(-1);
	m_iOptsCount = 0;
}

void CAddGameDialog::AddGameOpts()
{
	AfxGetEVLive()->GetGameOptions(m_pGameOpts, m_iOptsCount);

	// 先删除再添加;
	for (int i = 0; i < m_iOptsCount; i++)
	{
		CString strClass = CA2CT(m_pGameOpts[i].strClass);
		CString strTitle = CA2CT(m_pGameOpts[i].strTitle);
		CString strItemText;
		strItemText.Format(_T("[%s]: %s"), strClass, strTitle);
		m_cmbGameOpts.InsertString(i, strItemText);
	}
}

void CAddGameDialog::InitData()
{
	SetDlgItemText(IDOK, _T("确定"));

	GameInfo* pScene = new GameInfo();
	AfxGetEVLive()->GetSelSceneInfo(pScene);

	// 视频、音频;
	CStringA strTitle = pScene->strTitle;
	CStringA strClass = pScene->strClass;
	for (int i = 0; i < m_iOptsCount; i++)
	{
		if ((0 == strClass.Compare(m_pGameOpts[i].strClass)) &&
			(0 == strTitle.Compare(m_pGameOpts[i].strTitle)))
		{
			m_cmbGameOpts.SetCurSel(i);
			break;
		}
	}
}

void CAddGameDialog::OnOK()
{
	int iSel = m_cmbGameOpts.GetCurSel();
	if ( (iSel < 0) || (iSel >= m_cmbGameOpts.GetCount()) )
	{
		MessageBox(_T("请选择游戏源路径！"));
		return;
	}

	GameInfo& gameInfo = m_pGameOpts[iSel];
	switch (m_staScene)
	{
		case SCENE_ADD:
		{
			if (!AfxGetEVLive()->AddScene(&gameInfo))
			{
				//MessageBox(EV_GetErrorInfo().c_str());
				return;
			}
		}
		break;
	case SCENE_MODIFY:
		{
			if (!AfxGetEVLive()->ModifyScene(&gameInfo))
			{
				//MessageBox(EV_GetErrorInfo().c_str());
				return;
			}
		}
		break;
	default:
		break;
	}

	CDialogEx::OnOK();
}

void CAddGameDialog::OnCancel()
{
	CDialogEx::OnCancel();
}
