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

	/*-------------------------------------------------------------------------
	// 初始化Instance(UI显示前先调用);
	//
	// hMain:  主程序句柄;
	-------------------------------------------------------------------------*/
	virtual void InitInstance(IN HINSTANCE hMain) = 0;

	/*-------------------------------------------------------------------------
	// 设置直播视频参数;
	//
	// streamPara:  直播参数;
	-------------------------------------------------------------------------*/
	virtual void SetParameter(IN EVStreamerParameter& streamPara) = 0;

	/*-------------------------------------------------------------------------
	// 获取直播视频参数;
	//
	// streamPara:  直播参数;
	-------------------------------------------------------------------------*/
	virtual void GetParameter(OUT EVStreamerParameter& streamPara) = 0;

	/*-------------------------------------------------------------------------
	// 初始化视频窗口;
	//
	// hMain:  主窗口句柄;
	// hVideo: 视频窗口句柄;
	// bRemember: 是否记忆上次场景源;
	-------------------------------------------------------------------------*/
	virtual void InitVideoWnd(HWND hMain, HWND hVideo, bool bRemember) = 0;

	/*-------------------------------------------------------------------------
	// 准备推流;
	//
	// strLID
	// strKey
	-------------------------------------------------------------------------*/
	virtual void PrepareStream(/*IN const char* strLID, IN const char* strKey*/ ) = 0;

	/*-------------------------------------------------------------------------
	// 开始推流;
	//
	-------------------------------------------------------------------------*/
	virtual void StartStream() = 0;

	/*-------------------------------------------------------------------------
	// 停止推流;
	//
	-------------------------------------------------------------------------*/
	virtual void StopStream() = 0;

	/*-------------------------------------------------------------------------
	//	选中场景源操作;

		SceneItemsState_MoveUp = 0,			// 上移;
		SceneItemsState_MoveDown,			// 下移;
		SceneItemsState_MoveToTop,			// 置顶;
		SceneItemsState_MoveToBottom,		// 置底;
		SceneItemsState_MoveToFull,			// 全屏;
	-------------------------------------------------------------------------*/
	virtual void SetSelItemState(SceneItemsState itemState) = 0;

	/*-------------------------------------------------------------------------
	// 选中场景源:删除;
	//
	-------------------------------------------------------------------------*/
	virtual void DeleteSelItem() = 0;

	/*-------------------------------------------------------------------------
	// 游戏源: 获取游戏源列表;
	//
	// pGameOpts: 游戏源数组;
	// iCount: 游戏源个数;
	-------------------------------------------------------------------------*/
	virtual bool GetGameOptions(OUT GameInfo* pGameOpts, OUT int& iCount) = 0;

	/*-------------------------------------------------------------------------
	// 场景添加;
	//
	// pSceneInfo: 场景信息;
	-------------------------------------------------------------------------*/
	virtual bool AddScene(IN SceneInfo* pSceneInfo) = 0;

	/*-------------------------------------------------------------------------
	// 场景编辑;
	//
	// pSceneInfo: 场景信息;
	-------------------------------------------------------------------------*/
	virtual bool ModifyScene(IN SceneInfo* pSceneInfo) = 0;

	/*-------------------------------------------------------------------------
	// 麦克风音量: 设置;
	//
	-------------------------------------------------------------------------*/
	virtual void SetMicropVolume(float fValue) = 0;

	/*-------------------------------------------------------------------------
	// 麦克风音量: 获取;
	//
	-------------------------------------------------------------------------*/
	virtual float GetMicropVolume() = 0;

	/*-------------------------------------------------------------------------
	// 扬声器(桌面)音量:设置;
	//
	-------------------------------------------------------------------------*/
	virtual void SetDesktopVolume(float fValue) = 0;

	/*-------------------------------------------------------------------------
	// 扬声器(桌面)音量:获取;
	//
	-------------------------------------------------------------------------*/
	virtual float GetDesktopVolume() = 0;

	/*-------------------------------------------------------------------------
	// 获取可用的视频设备列表;
	//
	// pVideoDev: 视频设备数组;
	// iDevCount: 视频设备个数;
	-------------------------------------------------------------------------*/
	virtual bool GetVideoDevices(OUT DevicesInfo* pVideoDev, OUT int& iDevCount) = 0;

	/*-------------------------------------------------------------------------
	// 获取可用的音频设备列表;
	//
	// pVideoDev: 音频设备数组;
	// iDevCount: 音频设备个数;
	-------------------------------------------------------------------------*/
	virtual bool GetAudioDevices(OUT DevicesInfo* pVideoDev, OUT int& iDevCount) = 0;

	/*-------------------------------------------------------------------------
	// 获取选中的场景源类型;
	//
	// pClass: 场景源类型;
	-------------------------------------------------------------------------*/
	virtual bool GetSelSceneClass(OUT char* pClass) = 0;

	/*-------------------------------------------------------------------------
	// 获取选中的场景源信息;
	//
	// pScene: 场景源信息;
	-------------------------------------------------------------------------*/
	virtual bool GetSelSceneInfo(OUT SceneInfo* pScene) = 0;

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// EVAgora

	// 加入频道(连麦、视频会议使用);
	virtual int JoinChannel(const char* lpChannelName, const char* info, const unsigned int uid = 0, const char* channelKey = NULL) = 0;

	virtual int ConfigurePublisher(const EVPublisherConfiguration& config) = 0;
	// 离开频道(连麦、视频会议使用);
	virtual int LeaveChannel() = 0;

	// ;
	virtual int renewChannelKey(const char* channelKey) = 0;

	// initialize the engine
	//virtual int initialize(const RtcEngineContext& context) = 0;

	// get the pointer of the device manager object.
	//virtual int queryInterface(INTERFACE_ID_TYPE iid, void** inter) = 0;

	// get the version information of the SDK
	//virtual const char* getVersion(int* build) = 0;

	// get the version information of the SDK
	//virtual const char* getErrorDescription(int code) = 0;

	virtual int SetChannelProfile(EV_CHANNEL_PROFILE_TYPE profile) = 0;
	virtual int SetClientRole(EV_CLIENT_ROLE_TYPE role, const char* permissionKey) = 0;

	virtual int SetVideoProfile(EV_VIDEO_PROFILE_TYPE profile, bool swapWidthAndHeight) = 0;

	// start the echo testing, if every thing goes well you can hear your echo from the server
	//virtual int startEchoTest() = 0;

	// stop the echo testing
	//virtual int stopEchoTest() = 0;

	// start/stop the network testing
	virtual int EnableLastmileTest(bool bEnable) = 0;

	// enable/disable local and remote video showing
	virtual int EnableVideo(bool bEnable) = 0;

	// enable/disable local and remote video showing
	virtual int EnableDauleStream(bool bEnable) = 0;

	// SetRemoteStreamType;
	virtual int SetRemoteStreamType(UINT nUID, EV_REMOTE_VIDEO_STREAM_TYPE nType) = 0;

	// start the local video previewing
	virtual int StartPreview() = 0;

	// StartScreenCapture
	virtual int StartScreenCapture(HWND hCaptureWnd) = 0;

	// stop the local video previewing
	virtual int StopPreview() = 0;

	// set the remote video canvas
	virtual int SetupRemoteVideo(const EVVideoCanvas& canvas) = 0;

	// set the local video canvas
	virtual int SetupLocalVideo(const EVVideoCanvas& canvas) = 0;

	virtual int SetVideoCompositingLayout(const EVVideoCompositingLayout& sei) = 0;

	// get self call id in the current channel
	//virtual int getCallId(char* callId) = 0;

	//virtual int rate(const char* callId, int rating, const char* description) = 0;
	//virtual int complain(const char* callId, const char* description) = 0;

	// register a packet observer while the packet arrived or ready to be sent, the observer can touch the packet data
	//virtual int registerPacketObserver(IPacketObserver* observer) = 0;

	// Specify encryption mode of AES encryption algorithm.
	//virtual int setEncryptionMode(const char* encryptionMode) = 0;

	// Specify encryption secret enables built-in encryption function. Leaving channel will clear the secret specified in last channel;
	//virtual int setEncryptionSecret(const char* secret) = 0;

	//virtual int createDataStream(int* streamId, bool reliable, bool ordered) = 0;
	//virtual int sendStreamMessage(int streamId, const char* data, size_t length) = 0;

	//
	virtual void CreateChannel(IN const char* appid, IN const char* channel_id, IN const int type = 1, IN const int record = 1) = 0;

	//  uid不是必选 uid<0 请求服务器不必传入此值
	virtual void UserAuth(const char* appid, const char* channel_id, unsigned int uid, bool bNoUseUid=true) = 0;//
	virtual void LeaveEVChannel(IN const char* appid, IN const char* channel_id, IN unsigned int uid) = 0;
	//UserAuth返回的uid
	virtual void JoinEVChannel(IN const char* appid, IN const char* channel_id, IN const unsigned int uid, int role = 0) = 0;
	virtual void StartCommHeart(IN const char* appid, IN const char* channel_id, IN const unsigned int uid) = 0;
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