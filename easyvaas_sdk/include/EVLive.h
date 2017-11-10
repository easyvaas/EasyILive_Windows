#pragma once
#include "EVDefs.h"
#include "EVParameter.h"
#include "EVLiveCallBack.h"

// client ���ʵĳ�����;
class CEVLive
{
public:
	CEVLive(){};
	virtual ~CEVLive(){};

	/*-------------------------------------------------------------------------
	// ��ʼ��SDK: App����SDKǰ �����ȵ��ô˽ӿ�;EVMessage Init
	//
	// strAppID:		AppID;
	// strAccessKey:	AccessKey;
	// strSecretKey:	SecretKey;
	// strUserData:		�û��Զ�����Ϣ;
	-------------------------------------------------------------------------*/
	virtual void InitSDK(IN char* strAppID, IN char* strAccessKey, IN char* strSecretKey, IN char* strUserData) = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EVAgora

	// ����Ƶ��(������Ƶ����ʹ��);
	//
	virtual int JoinChannel(IN const char* appid, IN const char* channel_id="", IN unsigned int uid=0, EVLive_User_Role useRole = EVLIVE_USER_ROLE_BROADCASTER, IN const int type = 1, IN const int record = 1) = 0;
	virtual void ConfigurePublisher(const EVPublisherConfiguration& config) = 0;
// 	// �뿪Ƶ��(������Ƶ����ʹ��);

	virtual int LeaveChannel() = 0;
	//virtual void LeaveEVChannel(IN const char* appid, IN const char* channel_id, IN unsigned int uid) = 0;
	// ;
	virtual int renewChannelKey(const char* channelKey) = 0;

	//virtual int SetChannelProfile(EV_CHANNEL_PROFILE_TYPE profile) = 0;
	virtual int SetVideoProfile(EV_VIDEO_PROFILE_TYPE profile, bool swapWidthAndHeight) = 0;

	// start/stop the network testing
	virtual int EnableLastmileTest(bool bEnable) = 0;

	// enable/disable local and remote video showing
	virtual int EnableVideo(bool bEnable) = 0;

	virtual int StartPreview() = 0;
	virtual int StopPreveiw() = 0;
	// enable/disable local and remote video showing
	virtual int EnableDauleStream(bool bEnable) = 0;

	// SetRemoteStreamType;
	virtual int SetRemoteStreamType(UINT nUID, EV_REMOTE_VIDEO_STREAM_TYPE nType) = 0;

	// StartScreenCapture
	virtual int StartScreenCapture(HWND hCaptureWnd, int nCapFPS = 15, LPCRECT lpCapRect = NULL) = 0;
	virtual int StopScreenCapture() = 0;
	virtual bool IsScreenCapture() = 0;
	virtual int EnableLocalRender(bool bEnable) = 0;

	// set the remote video canvas
	virtual int SetupRemoteVideo(const EVVideoCanvas& canvas) = 0;

	//
	virtual int SetLocalPreviewMirror(bool bEnable) = 0;
	// set the local video canvas
	virtual int SetupLocalVideo(const EVVideoCanvas& canvas) = 0;

	virtual int SetVideoCompositingLayout(const EVVideoCompositingLayout& sei) = 0;
	virtual int ClearVideoCompositingLayout() = 0;

	virtual EV_LIVE_TYPE GetEVLiveType() = 0;
	virtual void GetRealComositionLayoutRect(RECT& rcComositionLayout) = 0;


	virtual void GetCurCameraID(std::string& cameraId) = 0;
	virtual BOOL SetCurCamera(const char* deviceID) = 0;
	virtual std::string GetCurRecordDeviceID() = 0;
	virtual BOOL SetCurRecordDevice(const char*  deviceID) = 0;
	//virtual BOOL GetCameraDevices(std::map<std::string, std::string>& mapVideos) = 0;
	virtual BOOL hasCamera() = 0;
	virtual BOOL GetCameraDevices(OUT DevicesInfo* pVideoDev, OUT int& iDevCount, int maxDeviceCount) = 0;
	virtual BOOL GetRecordingDevices(OUT DevicesInfo* pAudioDev, OUT int& iDevCount, int maxDeviceCount) = 0;
	virtual EV_VIDEO_PROFILE_TYPE GetVideoProfile(SIZE sz, int fps) = 0;

	//��������Զ����Ƶ
	//�÷�����������/��ֹ����Զ���û�����Ƶ������������Զ���û����о�����񡣸÷�����Ӱ����Ƶ�������Ľ��գ�ֻ�ǲ�������Ƶ����
	virtual int MuteAllRemoteAudioStreams(bool mute) = 0;
	//����ָ���û���Ƶ
	//����ָ��Զ���û�/��ָ��Զ���û�ȡ����������������������/��ֹ����Զ���û�����Ƶ�����÷�����Ӱ����Ƶ�������Ľ��գ�ֻ�ǲ�������Ƶ����
	virtual int MuteRemoteAudioStream(unsigned int uid, bool mute) = 0;
	//���Լ�����
	//����/ȡ���������÷�����������/��ֹ�����緢�ͱ�����Ƶ����
	//�÷�����Ӱ��¼��״̬����û�н�����˷硣
	virtual BOOL MuteLocalAudio(BOOL bMuted = TRUE) = 0;
	virtual BOOL IsLocalAudioMuted() = 0;
	//�÷�����Ӱ�챾����Ƶ����ȡ��û�н�������ͷ��

	//��ָͣ��Զ����Ƶ�� 
	//�÷�����������/��ֹ���������˵���Ƶ�����÷�����Ӱ����Ƶ�������Ľ��գ�ֻ�ǲ�������Ƶ����
	virtual int MuteRemoteVideoStream(unsigned int uid, bool mute) = 0;
	//��ͣ����Զ����Ƶ��
	//�÷�����������/��ֹ����ָ����Զ����Ƶ�����÷�����Ӱ����Ƶ�������Ľ��գ�ֻ�ǲ�������Ƶ����
	//True  : ֹͣ����ָ���û�����Ƶ��
	//False : ������ָ���û�����Ƶ��
	virtual int MuteAllRemoteVideoStreams(bool mute) = 0;
	//mute 
	//True : �����ͱ�����Ƶ��
	//False: ���ͱ�����Ƶ��
	virtual BOOL MuteLocalVideo(BOOL bMuted = TRUE) = 0;
	virtual BOOL IsLocalVideoMuted() = 0;
};

class EVLIVE_API CEVLiveFactory
{
public:
	// ����ʱ,��Ҫ���ݻص��ӿ�;
	static void CreateInstance(CEVLiveCallBack* pCallBack);

	// ��ȡ m_pInstance;
	static CEVLive* GetInstance();

	// �ͷ� m_pInstance;
	static void ReleaseInstance();

private:
	static CEVLive* m_pInstance;
};

#define AfxGetEVLive	CEVLiveFactory::GetInstance

// �ⲿ����ʾ���� AfxGetEVLive()->StartStream();