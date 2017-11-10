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
	ON_CBN_SELCHANGE(IDC_CAMERA_VIDEO_COMBO, &CAddCameraDialog::OnSelchangeCameraVideoCombo)
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
	int iFPSSel = m_cmbRatio.GetCurSel();
	int ret = -1;

	CString strFps;
	m_cmbFps.GetWindowText(strFps);
	int iFps = _ttoi(strFps);

	if (iFPSSel < m_vecResolutions.size())
	{
		SIZE sz = m_vecResolutions[iFPSSel];
		m_curVideoProfile = AfxGetEVLive()->GetVideoProfile(sz, iFps);
	}
	DisplayOutLine();
	return TRUE;
}


void CAddCameraDialog::OnOK()
{
	CameraInfo cameraInfo;
	int iVideoSel = m_cmbVideo.GetCurSel();
	int iAudioSel = m_cmbAudio.GetCurSel();
	bool bVideoChanged = false;
	bool bAudioChanged = false;
	if (iVideoSel > -1 && iVideoSel < m_iVedioDevCount)
	{
		strcpy_s(cameraInfo.strVideoName, m_pArrVedioDev[iVideoSel].strDevName);
		strcpy_s(cameraInfo.strVideoID, m_pArrVedioDev[iVideoSel].strDevID);

		std::string cameraId = "";
		cameraId.reserve(MAX_DEV_ID_LENGTH);
		AfxGetEVLive()->GetCurCameraID(cameraId);
		if (cameraId.length()>0 && cameraId.compare(cameraInfo.strVideoID) != 0)
		{
			bVideoChanged = true;
		}
	}


	if (iAudioSel > -1 && iAudioSel < m_iAudioDevCount)
	{
		strcpy_s(cameraInfo.strAudioName, m_pArrAudioDev[iAudioSel].strDevName);
		strcpy_s(cameraInfo.strAudioID, m_pArrAudioDev[iAudioSel].strDevID);

		std::string micId = "";
		micId.reserve(MAX_DEV_ID_LENGTH);
		AfxGetEVLive()->GetCurCameraID(micId);
		if (micId.length()>0 && micId.compare(cameraInfo.strAudioID) != 0)
		{
			bAudioChanged = true;
		}
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

	int iUserSet = 1;/*m_btnCustom.GetCheck() ? 1 : 0;*/
	CString strFps;
	m_cmbFps.GetWindowText(strFps);
	cameraInfo.iFps = _ttoi(strFps);

	CString strOutForm;
	m_cmbOutForm.GetWindowText(strOutForm);
	strcpy_s(cameraInfo.strOutForm, CT2CA(strOutForm.GetString()));

	if (bVideoChanged)
	{
		AfxGetEVLive()->SetCurCamera(cameraInfo.strVideoID);
	}

	if (bAudioChanged)
	{
		AfxGetEVLive()->SetCurRecordDevice(cameraInfo.strAudioID);
	}
	
	int iFPSSel = m_cmbRatio.GetCurSel();
	int ret = -1;
	if (iFPSSel < m_vecResolutions.size())
	{
		SIZE sz = m_vecResolutions[iFPSSel];
		EV_VIDEO_PROFILE_TYPE type = AfxGetEVLive()->GetVideoProfile(sz, cameraInfo.iFps);
		ret = AfxGetEVLive()->SetVideoProfile(type, false);

		if (m_curVideoProfile != type)
			bChangedVideoProfile = true;
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
	if (1/*m_btnCustom.GetCheck()*/)
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
	int i = 0;
	AfxGetEVLive()->GetCameraDevices(m_pArrVedioDev, m_iVedioDevCount, MAX_DEVICESCOUNT);
	for (int i = 0; i < m_iVedioDevCount; i++)
	{
		TCHAR szDeviceName[EV_MAX_DEVICE_ID_LENGTH] = { 0 };
		::MultiByteToWideChar(CP_UTF8, 0, m_pArrVedioDev[i].strDevName, -1, szDeviceName, EV_MAX_DEVICE_ID_LENGTH);
		m_cmbVideo.InsertString(i, szDeviceName);
		m_mapVideo[(m_pArrVedioDev[i].strDevID)] = m_pArrVedioDev[i].strDevName;
		m_vecVideoIds.push_back((m_pArrVedioDev[i].strDevID));
	}
	m_cmbVideo.SetCurSel(0);

	// 音频;

	AfxGetEVLive()->GetRecordingDevices(m_pArrAudioDev, m_iAudioDevCount, MAX_DEVICESCOUNT);
	for (int i = 0; i < m_iAudioDevCount; i++)
	{
		TCHAR szDeviceName[EV_MAX_DEVICE_ID_LENGTH] = { 0 };
		::MultiByteToWideChar(CP_UTF8, 0, m_pArrAudioDev[i].strDevName, -1, szDeviceName, EV_MAX_DEVICE_ID_LENGTH);
		m_cmbAudio.InsertString(i, szDeviceName);
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
	arrRatio.Add(_T("120 x 120"));

	arrRatio.Add(_T("320 x 180"));
	arrRatio.Add(_T("180 x 180"));
	arrRatio.Add(_T("240 x 180"));
	
	arrRatio.Add(_T("320 x 240"));
	arrRatio.Add(_T("240 x 240"));
	arrRatio.Add(_T("424 x 240"));
	
	arrRatio.Add(_T("640 x 360"));
	arrRatio.Add(_T("360 x 360"));
	arrRatio.Add(_T("480 x 360"));

	arrRatio.Add(_T("640 x 480"));
	arrRatio.Add(_T("480 x 480"));
	arrRatio.Add(_T("864 x 480"));

	arrRatio.Add(_T("1280 x 720"));
	arrRatio.Add(_T("960 x 720"));

	arrRatio.Add(_T("1920 x 1080"));

	arrRatio.Add(_T("2560 x 1440"));

	arrRatio.Add(_T("3840 x 2160"));

	//EV_VIDEO_PROFILE_DEFAULT 对应的是640 x 360
	for (int i = 0; i < arrRatio.GetCount(); i++)
	{
		m_cmbRatio.InsertString(i, arrRatio.GetAt(i));
		if (0 == arrRatio.GetAt(i).CompareNoCase(_T("640 x 360")))
		{
			m_cmbRatio.SetCurSel(i);
		}
	}

	SIZE sz = { 160, 120 };
	m_vecResolutions.push_back(sz);
	sz = { 120, 120 };
	m_vecResolutions.push_back(sz);

	sz = { 320, 180 };
	m_vecResolutions.push_back(sz);
	sz = { 180, 180 };
	m_vecResolutions.push_back(sz);
	sz = { 240, 180 };
	m_vecResolutions.push_back(sz);

	sz = { 320, 240 };
	m_vecResolutions.push_back(sz);
	sz = { 240, 240 };
	m_vecResolutions.push_back(sz);
	sz = { 424, 240 };
	m_vecResolutions.push_back(sz);

	sz = { 640, 360 };
	m_vecResolutions.push_back(sz);
	sz = { 360, 360 };
	m_vecResolutions.push_back(sz);
	sz = { 480, 360 };
	m_vecResolutions.push_back(sz);

	sz = { 640, 480 };
	m_vecResolutions.push_back(sz);
	sz = { 480, 480 };
	m_vecResolutions.push_back(sz);
	sz = { 864, 480 };
	m_vecResolutions.push_back(sz);

	sz = { 1280, 720 };
	m_vecResolutions.push_back(sz);
	sz = { 960, 720 };
	m_vecResolutions.push_back(sz);
	
	sz = { 1920, 1080 };
	m_vecResolutions.push_back(sz);

	sz = { 2560, 1440 };
	m_vecResolutions.push_back(sz);

	sz = { 3840, 2160 };
	m_vecResolutions.push_back(sz);
	return true;
}

bool CAddCameraDialog::AddFpsOptions()
{
	CStringArray arrFps;
	arrFps.Add(_T("15"));
	//arrFps.Add(_T("20"));
	//arrFps.Add(_T("25"));
	arrFps.Add(_T("30"));
	//arrFps.Add(_T("35"));
	//arrFps.Add(_T("40"));

	for (int i = 0; i < arrFps.GetCount(); i++)
	{
		m_cmbFps.InsertString(i, arrFps.GetAt(i));
		
	}
	m_cmbFps.SetCurSel(0);
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
// 
// 	CameraInfo* pScene = new CameraInfo();
// 	AfxGetEVLive()->GetSelSceneInfo(pScene);
// 
// 	// 视频、音频;
// 	CString strVedioName = CA2CT(pScene->strVideoName);
// 	for (int i = 0; i < m_cmbVideo.GetCount(); i++)
// 	{
// 		CString strItemText;
// 		m_cmbVideo.GetLBText(i, strItemText);
// 		if (0 == strVedioName.CompareNoCase(strItemText))
// 		{
// 			m_cmbVideo.SetCurSel(i);
// 			break;
// 		}
// 	}
// 
// 	CString strAudioName = CA2CT(pScene->strAudioName);
// 	for (int i = 0; i < m_cmbAudio.GetCount(); i++)
// 	{
// 		CString strItemText;
// 		m_cmbAudio.GetLBText(i, strItemText);
// 		if (0 == strAudioName.CompareNoCase(strItemText))
// 		{
// 			m_cmbAudio.SetCurSel(i);
// 			break;
// 		}
// 	}
// 
// 	// 镜头方向;
// 	int iFilpHor = FLIPTYPE_HOR & pScene->iFlipType ? 1 : 0;
// 	m_btnHor.SetCheck(iFilpHor);
// 	int iFilpVer = FLIPTYPE_VER & pScene->iFlipType ? 1 : 0;
// 	m_btnVer.SetCheck(iFilpVer);
// 
// 	// 自定义设置;
// 	int bUserSet = pScene->iCustomSet;
// 	m_btnCustom.SetCheck(bUserSet);
// 
// 	int iRatioWidth = pScene->iWidth;
// 	int iRatioHeight = pScene->iHeight;
// 	CString strRatio;
// 	strRatio.Format(_T("%d x %d"), iRatioWidth, iRatioHeight);
// 	for (int i = 0; i < m_cmbRatio.GetCount(); i++)
// 	{
// 		CString strItemText;
// 		m_cmbRatio.GetLBText(i, strItemText);
// 		if (0 == strRatio.Compare(strItemText))
// 		{
// 			m_cmbRatio.SetCurSel(i);
// 			break;
// 		}
// 	}
// 
// 	CString strFps;
// 	strFps.Format(_T("%d"), pScene->iFps);
// 	for (int i = 0; i < m_cmbFps.GetCount(); i++)
// 	{
// 		CString strItemText;
// 		m_cmbFps.GetLBText(i, strItemText);
// 		if (0 == strFps.Compare(strItemText))
// 		{
// 			m_cmbFps.SetCurSel(i);
// 			break;
// 		}
// 	}
// 
// 	CString strOutForm = CA2CT(pScene->strOutForm);
// 	for (int i = 0; i < m_cmbOutForm.GetCount(); i++)
// 	{
// 		CString strItemText;
// 		m_cmbOutForm.GetLBText(i, strItemText);
// 		if (0 == strOutForm.Compare(strItemText))
// 		{
// 			m_cmbOutForm.SetCurSel(i);
// 			break;
// 		}
// 	}
// 	
// 	delete pScene;
// 	pScene = NULL;
}



void CAddCameraDialog::OnSelchangeCameraVideoCombo()
{
	//AddRatioOptions();
}
