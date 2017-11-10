#pragma once
#include <string>
#include <map>
#include <vector>
#define MAX_DEVICESCOUNT	20
// ����ͷ;
class CAddCameraDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CAddCameraDialog)

public:
	CAddCameraDialog(SceneStatus status = SCENE_ADD, CWnd* pParent = NULL);
	virtual ~CAddCameraDialog();

	// �Ի�������;
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

	bool AddDevicesOptions();				// �����Ƶ����Ƶѡ����;
	bool RemoveAllDevicesOptions();			// ɾ����Ƶ����Ƶѡ����;
	bool AddRatioOptions();					// ��ӷֱ���ѡ����;
	bool AddFpsOptions();					// ���FPSѡ����;
	bool AddOutformOptions();				// ��������ʽѡ����;

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

	// �豸�б�;
 	DevicesInfo*		m_pArrVedioDev;
 	DevicesInfo*		m_pArrAudioDev;
	std::map<std::string, std::string> m_mapVideo;

	std::map<std::string, std::string> m_mapAudio;
	std::map<std::string,std::vector<SIZE> > m_mapVideoResolutions;
	//Ϊ�˺���Ͽ��������Ӧ
 	std::vector<std::string> m_vecVideoIds;
	std::vector<SIZE>  m_vecResolutions; //
// 	std::vector<std::string> m_vecRecordingIds;

	int					m_iVedioDevCount;
	int					m_iAudioDevCount;

	EV_VIDEO_PROFILE_TYPE m_curVideoProfile;
public:
	afx_msg void OnSelchangeCameraVideoCombo();
	//�û�ѡ�еķֱ�������
	int m_curResolutionIndex = 0;
	//�û�ѡ�е�fps����
	int m_curFpsIndex = 0;
	bool bChangedVideoProfile;
};