#pragma once
#include "EVDefs.h"
#include "EVParameter.h"
#include "EVLiveCallBack.h"

// client 访问的抽象类;
class CEVLive
{
public:
	CEVLive(){};
	virtual ~CEVLive(){};

	/*-------------------------------------------------------------------------
	// 初始化SDK: App调用SDK前 必须先调用此接口;EVMessage Init
	//
	// strAppID:		AppID;
	// strAccessKey:	AccessKey;
	// strSecretKey:	SecretKey;
	// strUserData:		用户自定义信息;
	-------------------------------------------------------------------------*/
	virtual void InitSDK(IN char* strAppID, IN char* strAccessKey, IN char* strSecretKey, IN char* strUserData) = 0;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EVAgora

	// 加入频道(连麦、视频会议使用);
	//
	virtual int JoinChannel(IN const char* appid, IN const char* channel_id="", IN unsigned int uid=0, EVLive_User_Role useRole = EVLIVE_USER_ROLE_BROADCASTER, IN const int type = 1, IN const int record = 1) = 0;
	virtual void ConfigurePublisher(const EVPublisherConfiguration& config) = 0;
// 	// 离开频道(连麦、视频会议使用);

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

	//静音所有远端音频
	//该方法用于允许/禁止播放远端用户的音频流，即对所有远端用户进行静音与否。该方法不影响音频数据流的接收，只是不播放音频流。
	virtual int MuteAllRemoteAudioStreams(bool mute) = 0;
	//静音指定用户音频
	//静音指定远端用户/对指定远端用户取消静音。本方法用于允许/禁止播放远端用户的音频流。该方法不影响音频数据流的接收，只是不播放音频流。
	virtual int MuteRemoteAudioStream(unsigned int uid, bool mute) = 0;
	//将自己静音
	//静音/取消静音。该方法用于允许/禁止往网络发送本地音频流。
	//该方法不影响录音状态，并没有禁用麦克风。
	virtual BOOL MuteLocalAudio(BOOL bMuted = TRUE) = 0;
	virtual BOOL IsLocalAudioMuted() = 0;
	//该方法不影响本地视频流获取，没有禁用摄像头。

	//暂停指定远端视频流 
	//该方法用于允许/禁止播放所有人的视频流。该方法不影响视频数据流的接收，只是不播放视频流。
	virtual int MuteRemoteVideoStream(unsigned int uid, bool mute) = 0;
	//暂停所有远端视频流
	//该方法用于允许/禁止播放指定的远端视频流。该方法不影响视频数据流的接收，只是不播放视频流。
	//True  : 停止播放指定用户的视频流
	//False : 允许播放指定用户的视频流
	virtual int MuteAllRemoteVideoStreams(bool mute) = 0;
	//mute 
	//True : 不发送本地视频流
	//False: 发送本地视频流
	virtual BOOL MuteLocalVideo(BOOL bMuted = TRUE) = 0;
	virtual BOOL IsLocalVideoMuted() = 0;
};

class EVLIVE_API CEVLiveFactory
{
public:
	// 创建时,需要传递回调接口;
	static void CreateInstance(CEVLiveCallBack* pCallBack);

	// 获取 m_pInstance;
	static CEVLive* GetInstance();

	// 释放 m_pInstance;
	static void ReleaseInstance();

private:
	static CEVLive* m_pInstance;
};

#define AfxGetEVLive	CEVLiveFactory::GetInstance

// 外部调用示例： AfxGetEVLive()->StartStream();