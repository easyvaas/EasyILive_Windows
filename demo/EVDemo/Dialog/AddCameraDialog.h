#pragma once
#include <string>
#include <map>
#include <vector>
#define MAX_DEVICESCOUNT	20
// 摄像头;
class CAddCameraDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddCameraDialog)

public:
	CAddCameraDialog(SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddCameraDialog();

	// 对话框数据;
	enum { IDD = IDD_ADDCAMERADIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedCameraFreshButton();
	afx_msg void OnBnClickedCaneraCustomsetCheck();

	virtual void OnOK();
	virtual void OnCancel();

protected:

	void InitData();
	void DisplayOutLine();

	bool AddDevicesOptions();				// 添加视频、音频选择项;
	bool RemoveAllDevicesOptions();			// 删除视频、音频选择项;
	bool AddRatioOptions();					// 添加分辨率选择项;
	bool AddFpsOptions();					// 添加FPS选择项;
	bool AddOutformOptions();				// 添加输出格式选择项;

private:
	CComboBox			m_cmbVideo;
	CComboBox			m_cmbAudio;
	CButton				m_btnHor;
	CButton				m_btnVer;

	CButton				m_btnCustom;
	CComboBox			m_cmbRatio;
	CComboBox			m_cmbFps;
	CComboBox			m_cmbOutForm;

	SceneStatus			m_staScene;

	// 设备列表;
 	DevicesInfo*		m_pArrVedioDev;
 	DevicesInfo*		m_pArrAudioDev;
	std::map<std::string, std::string> m_mapVideo;

	std::map<std::string, std::string> m_mapAudio;
	std::map<std::string,std::vector<SIZE> > m_mapVideoResolutions;
	//为了和组合框的索引对应
 	std::vector<std::string> m_vecVideoIds;
	std::vector<SIZE>  m_vecResolutions; //
// 	std::vector<std::string> m_vecRecordingIds;

	int					m_iVedioDevCount;
	int					m_iAudioDevCount;

	EV_VIDEO_PROFILE_TYPE m_curVideoProfile;
public:
	afx_msg void OnSelchangeCameraVideoCombo();
	//用户选中的分辨率索引
	int m_curResolutionIndex = 0;
	//用户选中的fps索引
	int m_curFpsIndex = 0;
	bool bChangedVideoProfile;
};