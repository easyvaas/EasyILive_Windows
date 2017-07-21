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

	// 初始化OBS--回调;
	virtual void InitInstance_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 设置直播视频参数--回调;
	virtual void SetParameter_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 获取直播视频参数--回调;
	virtual void GetParameter_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 初始化视频窗口--回调;
	virtual void InitVideoWnd_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	//virtual void GetLID_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo, OUT char* strLid, OUT char* strKey);
	// 准备推流--回调;
	// 失败时,返回错误信息; 成功时,返回推流地址;
	virtual void PrepareStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 开始推流--回调;
	virtual void StartStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 停止推流--回调;
	virtual void StopStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	//	选中场景源操作--回调;
	virtual void SetSelItemState_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 选中场景源删除--回调;
	virtual void DeleteSelItem_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 游戏源: 获取游戏源列表--回调;
	virtual void GetGameOptions_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 场景添加--回调;
	virtual void AddScene_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 场景编辑--回调;
	virtual void ModifyScene_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 麦克风音量: 设置--回调;
	virtual void SetMicropVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 麦克风音量: 获取--回调;
	virtual void GetMicropVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 扬声器(桌面)音量:设置--回调;
	virtual void SetDesktopVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 扬声器(桌面)音量:获取--回调;
	virtual void GetDesktopVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 获取可用的视频设备列表--回调;
	virtual void GetVideoDevices_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 获取可用的音频设备列表--回调;
	virtual void GetAudioDevices_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 获取选中的场景源类型--回调;
	virtual void GetSelSceneClass_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

	// 获取选中的场景源信息--回调;
	virtual void GetSelSceneInfo_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 连麦回调;

	virtual void onJoinChannelSuccess(const char* channel, unsigned int uid, int elapsed);
	virtual void onRejoinChannelSuccess(const char* channel, unsigned int uid, int elapsed);
	virtual void onWarning(int warn, const char* msg);
	virtual void onError(int err, const char* msg);
	virtual void onAudioQuality(unsigned int uid, int quality, unsigned short delay, unsigned short lost);
	virtual void onAudioVolumeIndication(const EVAudioVolumeInfo* speakers, unsigned int speakerNumber, int totalVolume);

	virtual void onLeaveChannel(const EVRtcStats& stat);
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

	virtual void onCreateChannel(IN const bool& bSuccessed, IN const char* strErrInfo, IN const char* channel_id, IN const char* push_url);
	virtual void onUserAuth(IN const bool& bSuccessed, IN const char* strErrInfo, IN const char* auth_key, IN const unsigned int owner_id, const char* channel_key, const unsigned int uid);
	virtual void onLeaveEVChannel(IN const bool& bSuccessed, IN const char* strErrInfo);
	virtual void onJoinEVChannel(IN const bool& bSuccessed, IN const char* strErrInfo);
	virtual void onStartCommHeart(IN const bool& bSuccessed, IN const char* strErrInfo);
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EVMessage

	virtual void onGetMessageServer(const char * serverIp, int serverPort, const char * handShakeKey);
	virtual void onConnectError(const char * errorInfo);
	virtual void onConnected();

	// 加入topic成功，content内容为json字符串，包括私有数据、用户列表信息（用户列表、管理员列表、禁言列表、踢人列表）;
	virtual void onJoinOK(const char * topic, const char ** users, int userNum,
		const char ** privateInfo, int privateInfoNum,
		const char ** managers, int managersNum,
		const char ** shutups, int shutupsNum,
		const char ** kickusers, int kickusersNum);
	// 离开topic成功，content内容为json字符串，包括私有数据、用户列表信息（用户列表、管理员列表、禁言列表、踢人列表）;
	virtual void onLeaveOK(const char * content, const char * topic);

	// message为json格式字符串；type包括：观看人数、系统消息、礼物、弹幕、直播状态、标题更新等;
	virtual void onNewMessage(const char * message, const char * topic, const char * userData, const char * userId);
	// 新加入用户列表;
	virtual void onUserJoin(char ** users, int num);
	// 离开用户的列表;
	virtual void onUserLeave(char ** users, int num);
	// 点赞数;
	virtual void onLikeCount(int likeCount);
	// 评论数;
	virtual void onCommentCount(int likeCount);
	// 饭团数;
	virtual void onRiceRoll(int riceCount);
	// 正在观看数;
	virtual void onWatchingCount(int watchingCount);
	// 已观看数;
	virtual void onWatchedCount(int watchedCount);
	// 错误列表，见下面列表;
	virtual void onError(int errCode);
};