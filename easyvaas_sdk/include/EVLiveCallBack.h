#pragma once
#include <windows.h>
#include "EVDefs.h"

#ifdef EVLIVE_EXPORTS
#define EVLIVE_API __declspec(dllexport)
#else
#define EVLIVE_API __declspec(dllimport)
#endif

class EVLIVE_API CEVLiveCallBack
{
public:
	CEVLiveCallBack();
	virtual ~CEVLiveCallBack();

	// 初始化SDK--回调;
	virtual void InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo) = 0;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 连麦回调;

	virtual void onJoinChannelSuccess(const char* channel, unsigned int uid, int elapsed, const char* push_url) = 0;
	virtual void onJoinChannelError(const char* Error) = 0;
	virtual void onRejoinChannelSuccess(const char* channel, unsigned int uid, int elapsed) = 0;
	virtual void onWarning(int warn, const char* msg) = 0;
	virtual void onError(int err, const char* msg) = 0;
	virtual void onAudioQuality(unsigned int uid, int quality, unsigned short delay, unsigned short lost) = 0;
	virtual void onAudioVolumeIndication(const EVAudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume) = 0;

	virtual void onLeaveChannel(bool bSuccess, const char* strErrInfo, const EVRtcStats& stat) = 0;
	virtual void onRtcStats(const EVRtcStats& stat) = 0;
	virtual void onMediaEngineEvent(int evt) = 0;

	virtual void onAudioDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) = 0;
	virtual void onVideoDeviceStateChanged(const char* deviceId, int deviceType, int deviceState) = 0;

	virtual void onLastmileQuality(int quality) = 0;
	virtual void onFirstLocalVideoFrame(int width, int height, int elapsed) = 0;
	virtual void onFirstRemoteVideoDecoded(unsigned int uid, int width, int height, int elapsed) = 0;
	virtual void onFirstRemoteVideoFrame(unsigned int uid, int width, int height, int elapsed) = 0;
	virtual void onUserJoined(unsigned int uid, int elapsed) = 0;
	virtual void onUserOffline(unsigned int uid, EV_USER_OFFLINE_REASON_TYPE reason) = 0;
	virtual void onUserMuteAudio(unsigned int uid, bool muted) = 0;
	virtual void onUserMuteVideo(unsigned int uid, bool muted) = 0;
	virtual void onApiCallExecuted(const char* api, int error) = 0;

	virtual void onLocalVideoStats(const EVLocalVideoStats& stats) = 0;
	virtual void onRemoteVideoStats(const EVRemoteVideoStats& stats) = 0;
	virtual void onCameraReady() = 0;
	virtual void onVideoStopped() = 0;
	virtual void onConnectionLost() = 0;
	virtual void onConnectionInterrupted() = 0;

	virtual void onUserEnableVideo(unsigned int uid, bool enabled) = 0;

	virtual void onStartRecordingService(int error) = 0;
	virtual void onStopRecordingService(int error) = 0;
	virtual void onRefreshRecordingServiceStatus(int status) = 0;

	virtual void onStartCommHeart(IN const bool& bSuccessed, IN const char* strErrInfo, bool ownerOnline) = 0;

};