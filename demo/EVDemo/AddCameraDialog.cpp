#include "stdafx.h"
#include "Demo.h"
#include "AddCameraDialog.h"

// CAddCameraDialog 对话框
IMPLEMENT_DYNAMIC(CAddCameraDialog, CDialogEx)

CAddCameraDialog::CAddCameraDialog(SceneStatus status /*= SCENE_ADD*/, CWnd* pParent /*=NULL*/)
	: CDialogEx(CAddCameraDialog::IDD, pParent)
{
	m_staScene = status;

	m_pArrVedioDev = new DevicesInfo[MAX_DEVICESCOUNT];
	m_pArrAudioDev = new DevicesInfo[MAX_DEVICESCOUNT];
	m_iVedioDevCount = 0;
	m_iAudioDevCount = 0;
}

CAddCameraDialog::~CAddCameraDialog()
{
	delete[]m_pArrVedioDev;
	m_pArrVedioDev = NULL;
	delete[]m_pArrAudioDev;
	m_pArrAudioDev = NULL;
}

void CAddCameraDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CAMERA_VIDEO_COMBO, m_cmbVideo);
	DDX_Control(pDX, IDC_CAMERA_AUDIO_COMBO, m_cmbAudio);
	DDX_Control(pDX, IDC_CANERA_HOR_CHECK, m_btnHor);
	DDX_Control(pDX, IDC_CANERA_VER_CHECK, m_btnVer);
	DDX_Control(pDX, IDC_CANERA_CUSTOMSET_CHECK, m_btnCustom);
	DDX_Control(pDX, IDC_CANERA_RATIO_COMBO, m_cmbRatio);
	DDX_Control(pDX, IDC_CANERA_FPS_COMBO, m_cmbFps);
	DDX_Control(pDX, IDC_CANERA_OUTFORM_COMBO, m_cmbOutForm);
}


BEGIN_MESSAGE_MAP(CAddCameraDialog, CDialogEx)
	ON_BN_CLICKED(IDC_CAMERA_FRESH_BUTTON, &CAddCameraDialog::OnBnClickedCameraFreshButton)
	ON_BN_CLICKED(IDC_CANERA_CUSTOMSET_CHECK, &CAddCameraDialog::OnBnClickedCaneraCustomsetCheck)
END_MESSAGE_MAP()

BOOL CAddCameraDialog::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	AddDevicesOptions();				// 添加视频、音频选择项;
	AddRatioOptions();					// 添加分辨率选择项;
	AddFpsOptions();					// 添加FPS选择项;
	AddOutformOptions();				// 添加输出格式选择项;

	if (SCENE_MODIFY == m_staScene)
	{
		InitData();
	}

	DisplayOutLine();
	return TRUE;
}

void CAddCameraDialog::OnOK()
{
	CameraInfo cameraInfo;
	int iVideoSel = m_cmbVideo.GetCurSel();
	int iAudioSel = m_cmbAudio.GetCurSel();
	if (iVideoSel > -1 && iVideoSel < m_iVedioDevCount)
	{
		strcpy_s(cameraInfo.strVideoName, m_pArrVedioDev[iVideoSel].strDevName);
		strcpy_s(cameraInfo.strVideoID, m_pArrVedioDev[iVideoSel].strDevID);
	}
	if (iAudioSel > -1 && iAudioSel < m_iAudioDevCount)
	{
		strcpy_s(cameraInfo.strAudioName, m_pArrAudioDev[iAudioSel].strDevName);
		strcpy_s(cameraInfo.strAudioID, m_pArrAudioDev[iAudioSel].strDevID);
	}

	if ( 0 == strlen(cameraInfo.strVideoName) )
	{
		MessageBox(_T("请选择摄像头！"));
		return;
	}
	if (0 == strlen(cameraInfo.strVideoName) )
	{
		MessageBox(_T("请选择麦克风！"));
		return;
	}

	CString strRatio;
	m_cmbRatio.GetWindowText(strRatio);
	CString strCameraWidth, strCameraHeight;
	int iPos = strRatio.Find(_T("x"));
	if (iPos > -1)
	{
		strCameraWidth = strRatio.Left(iPos);
		strCameraHeight = strRatio.Right(strRatio.GetLength() - (iPos + 1));
		cameraInfo.iWidth = _ttoi(strCameraWidth);
		cameraInfo.iHeight = _ttoi(strCameraHeight);
	}

	cameraInfo.iFlipType = FLIPTYPE_NONE;
	if (m_btnHor.GetCheck())
	{
		cameraInfo.iFlipType |= FLIPTYPE_HOR;
	}
	if (m_btnVer.GetCheck())
	{
		cameraInfo.iFlipType |= FLIPTYPE_VER;
	}

	int iUserSet = m_btnCustom.GetCheck() ? 1 : 0;
	CString strFps;
	m_cmbFps.GetWindowText(strFps);
	cameraInfo.iFps = _ttoi(strFps);

	CString strOutForm;
	m_cmbOutForm.GetWindowText(strOutForm);
	strcpy_s(cameraInfo.strOutForm, CT2CA(strOutForm.GetString()));

	switch (m_staScene)
	{
		case SCENE_ADD:
		{
			if (!AfxGetEVLive()->AddScene(&cameraInfo))
			{
				//MessageBox(EV_GetErrorInfo().c_str());
				return;
			}
		}
		break;
	case SCENE_MODIFY:
		{
			if (!AfxGetEVLive()->ModifyScene(&cameraInfo))
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

void CAddCameraDialog::OnCancel()
{
	CDialogEx::OnCancel();
}

void CAddCameraDialog::OnBnClickedCameraFreshButton()
{
	RemoveAllDevicesOptions();
	AddDevicesOptions();
}


void CAddCameraDialog::OnBnClickedCaneraCustomsetCheck()
{
	DisplayOutLine();
}

void CAddCameraDialog::DisplayOutLine()
{
	if (m_btnCustom.GetCheck())
	{
		m_cmbRatio.EnableWindow(TRUE);
		m_cmbFps.EnableWindow(TRUE);
		m_cmbOutForm.EnableWindow(TRUE);
	}
	else
	{
		m_cmbRatio.EnableWindow(FALSE);
		m_cmbFps.EnableWindow(FALSE);
		m_cmbOutForm.EnableWindow(FALSE);
	}
}

bool CAddCameraDialog::AddDevicesOptions()
{
	// 视频;
	AfxGetEVLive()->GetVideoDevices(m_pArrVedioDev, m_iVedioDevCount);
	for (int i = 0; i < m_iVedioDevCount; i++)
	{
		m_cmbVideo.InsertString(i, CA2CT(m_pArrVedioDev[i].strDevName));
	}
	m_cmbVideo.SetCurSel(0);

	// 音频;
	AfxGetEVLive()->GetAudioDevices(m_pArrAudioDev, m_iAudioDevCount);
	for (int i = 0; i < m_iAudioDevCount; i++)
	{
		m_cmbAudio.InsertString(i, CA2CT(m_pArrAudioDev[i].strDevName));
	}
	m_cmbAudio.SetCurSel(0);

	return true;
}

bool CAddCameraDialog::RemoveAllDevicesOptions()
{
	for (int i = m_cmbVideo.GetCount() - 1; i >= 0; i--)
	{
		m_cmbVideo.DeleteString(i);
	}
	m_cmbVideo.SetCurSel(-1);

	for (int i = m_cmbAudio.GetCount() - 1; i >= 0; i--)
	{
		m_cmbAudio.DeleteString(i);
	}
	m_cmbAudio.SetCurSel(-1);

	return true;
}

bool CAddCameraDialog::AddRatioOptions()
{
	CStringArray arrRatio;
	arrRatio.Add(_T("160 x 120"));
	arrRatio.Add(_T("176 x 144"));
	arrRatio.Add(_T("320 x 176"));
	arrRatio.Add(_T("320 x 240"));
	arrRatio.Add(_T("432 x 240"));
	arrRatio.Add(_T("352 x 288"));
	arrRatio.Add(_T("544 x 288"));
	arrRatio.Add(_T("640 x 360"));
	arrRatio.Add(_T("752 x 416"));
	arrRatio.Add(_T("800 x 448"));
	arrRatio.Add(_T("640 x 480"));
	arrRatio.Add(_T("864 x 480"));
	arrRatio.Add(_T("960 x 544"));
	arrRatio.Add(_T("1024 x 576"));
	arrRatio.Add(_T("800 x 600"));
	arrRatio.Add(_T("1184 x 656"));
	arrRatio.Add(_T("960 x 720"));
	arrRatio.Add(_T("1280 x 720"));
	arrRatio.Add(_T("1280 x 960"));

	for (int i = 0; i < arrRatio.GetCount(); i++)
	{
		m_cmbRatio.InsertString(i, arrRatio.GetAt(i));
		if (0 == arrRatio.GetAt(i).CompareNoCase(_T("1280 x 720")) )
		{
			m_cmbRatio.SetCurSel(i);
		}
	}

	return true;
}

bool CAddCameraDialog::AddFpsOptions()
{
	CStringArray arrFps;
	arrFps.Add(_T("15"));
	arrFps.Add(_T("20"));
	arrFps.Add(_T("25"));
	arrFps.Add(_T("30"));
	arrFps.Add(_T("35"));
	arrFps.Add(_T("40"));

	for (int i = 0; i < arrFps.GetCount(); i++)
	{
		m_cmbFps.InsertString(i, arrFps.GetAt(i));
		if (0 == arrFps.GetAt(i).CompareNoCase(_T("25")))
		{
			m_cmbFps.SetCurSel(i);
		}
	}

	return true;
}

bool CAddCameraDialog::AddOutformOptions()
{
	CStringArray arrOutForm;
	arrOutForm.Add(_T("I420"));
	arrOutForm.Add(_T("RGB24"));

	for (int i = 0; i < arrOutForm.GetCount(); i++)
	{
		m_cmbOutForm.InsertString(i, arrOutForm.GetAt(i));
		if (0 == arrOutForm.GetAt(i).CompareNoCase(_T("I420")))
		{
			m_cmbOutForm.SetCurSel(i);
		}
	}

	return true;
}

void CAddCameraDialog::InitData()
{
	SetDlgItemText(IDOK, _T("确定"));

	CameraInfo* pScene = new CameraInfo();
	AfxGetEVLive()->GetSelSceneInfo(pScene);

	// 视频、音频;
	CString strVedioName = CA2CT(pScene->strVideoName);
	for (int i = 0; i < m_cmbVideo.GetCount(); i++)
	{
		CString strItemText;
		m_cmbVideo.GetLBText(i, strItemText);
		if (0 == strVedioName.CompareNoCase(strItemText))
		{
			m_cmbVideo.SetCurSel(i);
			break;
		}
	}

	CString strAudioName = CA2CT(pScene->strAudioName);
	for (int i = 0; i < m_cmbAudio.GetCount(); i++)
	{
		CString strItemText;
		m_cmbAudio.GetLBText(i, strItemText);
		if (0 == strAudioName.CompareNoCase(strItemText))
		{
			m_cmbAudio.SetCurSel(i);
			break;
		}
	}

	// 镜头方向;
	int iFilpHor = FLIPTYPE_HOR & pScene->iFlipType ? 1 : 0;
	m_btnHor.SetCheck(iFilpHor);
	int iFilpVer = FLIPTYPE_VER & pScene->iFlipType ? 1 : 0;
	m_btnVer.SetCheck(iFilpVer);

	// 自定义设置;
	int bUserSet = pScene->iCustomSet;
	m_btnCustom.SetCheck(bUserSet);

	int iRatioWidth = pScene->iWidth;
	int iRatioHeight = pScene->iHeight;
	CString strRatio;
	strRatio.Format(_T("%d x %d"), iRatioWidth, iRatioHeight);
	for (int i = 0; i < m_cmbRatio.GetCount(); i++)
	{
		CString strItemText;
		m_cmbRatio.GetLBText(i, strItemText);
		if (0 == strRatio.Compare(strItemText))
		{
			m_cmbRatio.SetCurSel(i);
			break;
		}
	}

	CString strFps;
	strFps.Format(_T("%d"), pScene->iFps);
	for (int i = 0; i < m_cmbFps.GetCount(); i++)
	{
		CString strItemText;
		m_cmbFps.GetLBText(i, strItemText);
		if (0 == strFps.Compare(strItemText))
		{
			m_cmbFps.SetCurSel(i);
			break;
		}
	}

	CString strOutForm = CA2CT(pScene->strOutForm);
	for (int i = 0; i < m_cmbOutForm.GetCount(); i++)
	{
		CString strItemText;
		m_cmbOutForm.GetLBText(i, strItemText);
		if (0 == strOutForm.Compare(strItemText))
		{
			m_cmbOutForm.SetCurSel(i);
			break;
		}
	}
	
	delete pScene;
	pScene = NULL;
}

