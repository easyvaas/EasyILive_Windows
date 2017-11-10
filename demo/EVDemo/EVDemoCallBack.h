#pragma once
#include "EVLiveCallBack.h"

enum
{
	WM_INITSDK_CALLBACK = WM_USER + 1300,		// 初始化SDK;
	WM_INITINSTANCE_CALLBACK,					// 初始化Instance;
	WM_INITVIDEOWND_CALLBACK,					// 初始化视频窗口;
	WM_PREPARESTREAM_CALLBACK,					// 准备推流;

	WM_ADDGAME_CALLBACK,						// 添加游戏源;
	WM_MODIFYGAME_CALLBACK,						// 编辑游戏源;
	WM_ADDMONITOR_CALLBACK,						// 添加截屏;
	WM_ADDTEXT_CALLBACK,						// 添加文本源;
	WM_MODIFYTEXT_CALLBACK,						// 编辑文本源;
	WM_ADDIMAGE_CALLBACK,						// 添加图片源;
	WM_MODIFYIMAGE_CALLBACK,					// 编辑图片源;
	WM_ADDCAMERA_CALLBACK,						// 添加摄像头;
	WM_MODIFYCAMERA_CALLBACK,					// 编辑摄像头;
//	WM_GETLIDANDKEY_SUCCESS,                    // 获取lid和key成功

	WM_MESSAGE_CONNECT = WM_USER + 1400,		// 消息系统连接;
	WM_MESSAGE_DISCONNECT,						// 消息系统断开;

	WM_STREAM_STOP = WM_USER + 1500
};

class CEVDemoCallBack : public CEVLiveCallBack
{
public:
	CEVDemoCallBack();
	virtual ~CEVDemoCallBack();

public:

	// 初始化SDK--回调;
	virtual void InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 连麦回调;

	virtual void onJoinChannelSuccess(const char* channel, unsigned int uid, int elapsed, const char* push_url);
	virtual void onJoinChannelError(const char* Error);
	virtual void onRejoinChannelSuccess(const char* channel, unsigned int uid, int elapsed);
	virtual void onWarning(int warn, const char* msg);
	virtual void onError(int err, const char* msg);
	virtual void onAudioQuality(unsigned int uid, int quality, unsigned short delay, unsigned short lost);
	virtual void onAudioVolumeIndication(const EVAudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume);

	virtual void onLeaveChannel(bool bSuccess, const char* strErrInfo, const EVRtcStats& stat);
	virtual void onRtcStats(const EVRtcStats& stat);
	virtual void onMediaEngineEvent(int evt);

	virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState);
	virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState);

	virtual void onLastmileQuality(int quality);
	virtual void onFirstLocalVideoFrame(int width, int height, int elapsed);
	virtual void onFirstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed);
	virtual void onFirstRemoteVideoFrame(unsigned int uid, int width, int height, int elapsed);
	virtual void onUserJoined(unsigned int uid, int elapsed);
	virtual void onUserOffline(unsigned int uid, EV_USER_OFFLINE_REASON_TYPE reason);
	virtual void onUserMuteAudio(unsigned int uid, bool muted);
	virtual void onUserMuteVideo(unsigned int uid, bool muted);
	virtual void onApiCallExecuted(const char* api, int error);

	virtual void onLocalVideoStats(const EVLocalVideoStats& stats);
	virtual void onRemoteVideoStats(const EVRemoteVideoStats& stats);
	virtual void onCameraReady();
	virtual void onVideoStopped();
	virtual void onConnectionLost();
	virtual void onConnectionInterrupted();

	virtual void onUserEnableVideo(unsigned int uid, bool enabled);

	virtual void onStartRecordingService(int error);
	virtual void onStopRecordingService(int error);
	virtual void onRefreshRecordingServiceStatus(int status);

	virtual void onStartCommHeart(IN const bool& bSuccessed, IN const char* strErrInfo, bool ownerOnline);
};