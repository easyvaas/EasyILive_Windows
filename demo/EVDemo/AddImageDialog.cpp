#include "stdafx.h"
#include "Demo.h"
#include "AddImageDialog.h"

IMPLEMENT_DYNAMIC(CAddImageDialog, CDialogEx)

CAddImageDialog::CAddImageDialog(SceneStatus status /*= SCENE_ADD*/, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddImageDialog::IDD, pParent)
{
	m_staScene = status;
}

CAddImageDialog::~CAddImageDialog()
{
}

void CAddImageDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IMAGE_PATH_EDIT, m_edtImagePath);
	DDX_Control(pDX, IDC_IAMGE_BKGALPHA_SLIDER, m_ctrlBkg);
}

BEGIN_MESSAGE_MAP(CAddImageDialog, CDialogEx)
	ON_BN_CLICKED(IDC_IMAGE_OPEN_BUTTON, &CAddImageDialog::OnBnClickedImageOpenButton)
END_MESSAGE_MAP()

void CAddImageDialog::OnOK()
{
	ImageInfo imageInfo;
	CString strPath;
	m_edtImagePath.GetWindowText(strPath);
	if (strPath.IsEmpty())
	{
		MessageBox(_T("请选择图片源！"));
		return;
	}
	strcpy_s(imageInfo.strPath, CT2CA(strPath.GetString()));
	imageInfo.iAlpha = m_ctrlBkg.GetPos();

	switch (m_staScene)
	{
		case SCENE_ADD:
		{
			if (!AfxGetEVLive()->AddScene(&imageInfo))
			{
				//MessageBox(EV_GetErrorInfo().c_str());
				return;
			}
		}
		break;
	case SCENE_MODIFY:
		{
			if (!AfxGetEVLive()->ModifyScene(&imageInfo))
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


void CAddImageDialog::OnCancel()
{
	CDialogEx::OnCancel();
}

void CAddImageDialog::OnBnClickedImageOpenButton()
{
	CString strFilter = _T("图片(*.bmp;*.jpg;*.jpeg;*.png;*.gif)|*.bmp;*.jpg;*.jpeg;*.png;*.gif||");
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, strFilter);
	if (dlg.DoModal() == IDOK)
	{
		CString strPath = dlg.GetPathName();
		m_edtImagePath.SetWindowText(strPath);
	}
}

BOOL CAddImageDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	if (SCENE_MODIFY == m_staScene)
	{
		InitData();
	}

	return TRUE;
}

void CAddImageDialog::InitData()
{
	SetDlgItemText(IDOK, _T("确定"));

	ImageInfo* pScene = new ImageInfo();
	AfxGetEVLive()->GetSelSceneInfo(pScene);

	// 图片路径;
	CString strImagePath = CA2CT(pScene->strPath);
	m_edtImagePath.SetWindowText(strImagePath);
	// 透明度;	
	int iAlpha = pScene->iAlpha;
	m_ctrlBkg.SetPos(iAlpha);
}