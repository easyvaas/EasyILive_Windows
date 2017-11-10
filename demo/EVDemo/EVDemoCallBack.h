#pragma once
#include "EVLiveCallBack.h"

enum
{
	WM_INITSDK_CALLBACK = WM_USER + 1300,		// ��ʼ��SDK;
	WM_INITINSTANCE_CALLBACK,					// ��ʼ��Instance;
	WM_INITVIDEOWND_CALLBACK,					// ��ʼ����Ƶ����;
	WM_PREPARESTREAM_CALLBACK,					// ׼������;

	WM_ADDGAME_CALLBACK,						// �����ϷԴ;
	WM_MODIFYGAME_CALLBACK,						// �༭��ϷԴ;
	WM_ADDMONITOR_CALLBACK,						// ��ӽ���;
	WM_ADDTEXT_CALLBACK,						// ����ı�Դ;
	WM_MODIFYTEXT_CALLBACK,						// �༭�ı�Դ;
	WM_ADDIMAGE_CALLBACK,						// ���ͼƬԴ;
	WM_MODIFYIMAGE_CALLBACK,					// �༭ͼƬԴ;
	WM_ADDCAMERA_CALLBACK,						// �������ͷ;
	WM_MODIFYCAMERA_CALLBACK,					// �༭����ͷ;
//	WM_GETLIDANDKEY_SUCCESS,                    // ��ȡlid��key�ɹ�

	WM_MESSAGE_CONNECT = WM_USER + 1400,		// ��Ϣϵͳ����;
	WM_MESSAGE_DISCONNECT,						// ��Ϣϵͳ�Ͽ�;

	WM_STREAM_STOP = WM_USER + 1500
};

class CEVDemoCallBack : public CEVLiveCallBack
{
public:
	CEVDemoCallBack();
	virtual ~CEVDemoCallBack();

public:

	// ��ʼ��SDK--�ص�;
	virtual void InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// ����ص�;

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