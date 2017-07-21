#pragma once
#include <windows.h>
#include <vector>
#include <map>
#include "EVDefs.h"

#define EV_WINDOW_CLASS         TEXT("EasyVideoWindowClass")
#define EV_VIDEO_CLASS			TEXT("EV_VIDEO_CLASS")

#define MONITOR_CAPTURE			"MonitorCaptureSource"
#define WINDOW_CAPTURE			"WindowCaptureSource"
#define DEVICE_CAPTURE			"DeviceCapture"
#define BITMAP_IMAGESOURCE		"BitmapImageSource"
#define GRAPHICS_CAPTURE		"GraphicsCapture"
#define TEXT_SOURCE				"TextSource"

#define MAX_GAMEOPTS_COUNT		100

enum
{
	OBS_REQUESTSTOP = WM_USER + 1,
	OBS_CALLHOTKEY,
	OBS_RECONNECT,
	OBS_SETSCENE,
	OBS_SETSOURCEORDER,
	OBS_SETSOURCERENDER,
	OBS_UPDATESTATUSBAR,
	OBS_NOTIFICATIONAREA,
	OBS_NETWORK_FAILED,
	OBS_CONFIGURE_STREAM_BUTTONS,

	WM_VEDIOWND_LBUTTONDOWN,					// 视频显示窗口-ldown;
	WM_VEDIOWND_RBUTTONDOWN,					// 视频显示窗口-lup;
	WM_VEDIOWND_LBUTTONUP,						// 视频显示窗口-rdown;
	WM_VEDIOWND_RBUTTONUP,						// 视频显示窗口-rup;

	WM_GRAPHNOTIFY = WM_USER + 1200,			// dshow视频采集消息;
	WM_RTMP_CONSTRAINED_UNCOMFORTABLE,			// 直播网络阻塞轻微;
	WM_RTMP_CONSTRAINED_DIFFICULT,				// 直播网络阻塞明显;
	WM_RTMP_CONSTRAINED_ABORT,					// 直播网络阻塞关闭;
	WM_RTMP_CONSTRAINED_SMOOTH,					// 直播网络通畅;
	WM_RTMP_TIME_DIFF_50_200,					// 直播网络缓冲延迟 < 200ms;
	WM_RTMP_TIME_DIFF_1S_2S,					// 直播网络缓冲延迟 > 1s;
	WM_RTMP_TIME_DIFF_2S_4S,					// 直播网络缓冲延迟 > 2s;
	WM_RTMP_TIME_DIFF_4S_9S,					// 直播网络缓冲延迟 > 4s;
	WM_RTMP_TIME_DIFF_9S_15S,					// 直播网络缓冲延迟 > 9s;
	WM_RTMP_TIME_DIFF_15S_30S,					// 直播网络缓冲延迟 > 15s;
	WM_RTMP_TIME_DIFF_30S_MAX,					// 直播网络缓冲延迟 > 30s;
};

class SceneInfo
{
public:
	SceneInfo(){};
	virtual ~SceneInfo(){};
};

class GameInfo : public SceneInfo
{
// 游戏源信息;
public:
	GameInfo()
	{
		strcpy_s(strTitle, "");
		strcpy_s(strClass, "");
		strcpy_s(strExePath, "");
		strcpy_s(strExeFile, "");
	}
	char	strTitle[255];				// 窗口标题;
	char	strClass[255];				// 窗口类名;
	HWND	hWnd;						// 窗口句柄;
	char	strExePath[255];			// 程序路径;
	char	strExeFile[255];			// 程序文件;
};

class MonitorsInfo : public SceneInfo
{
	// 截屏信息;
public:
	RECT rectMonitors;
};

class WindowsInfo : public SceneInfo
{
	// 窗口信息;
public:
	WindowsInfo()
	{
		strcpy_s(strTitle, "");
		strcpy_s(strClass, "");
		strcpy_s(strExePath, "");
		strcpy_s(strExeFile, "");
	}
	WindowsInfo(const GameInfo& infoGame)
	{
		strcpy_s(strTitle, infoGame.strTitle);
		strcpy_s(strClass, infoGame.strClass);
		strcpy_s(strExePath, infoGame.strExePath);
		strcpy_s(strExeFile, infoGame.strExeFile);
		hWnd = infoGame.hWnd;
	}
	char	strTitle[255];				// 窗口标题;
	char	strClass[255];				// 窗口类名;
	HWND	hWnd;						// 窗口句柄;
	char	strExePath[255];			// 程序路径;
	char	strExeFile[255];			// 程序文件;

	//int		i
};

class TextInfo : public SceneInfo
{// 文本源信息;
public:
	TextInfo()
	{
		strcpy_s(strContent, "");
		strcpy_s(strFontName, "微软雅黑");
		dwFontColor = 0xFF000000;
		iBold = 0;
		iItalic = 0;
		iUnderLine = 0;
		iVer = 0;
		iScrollSpeed = 0;

		iOutLineUsed = 0;
		dwOutLineColor = 0xFF000000;
		iOutLineSize = 0;
		iOutlineOpacity = 100;
	}

	char strContent[1000];		// 文本内容;
	char strFontName[100];		// 文本字体;
	DWORD dwFontColor;			// 文本颜色;
	int iBold;					// 粗体;
	int iItalic;				// 斜体;
	int iUnderLine;				// 下划线;
	int iVer;					// 垂直显示;
	int iScrollSpeed;			// 文本滚动速度;

	int iOutLineUsed;			// 文本外边框设置;
	DWORD dwOutLineColor;		// 文本外边框颜色;
	int iOutLineSize;			// 文本外边框厚度;
	int iOutlineOpacity;		// 文本外边框透明度;
};

class CameraInfo : public SceneInfo
{// 摄像头信息;
public:
	CameraInfo()
	{
		iWidth = 1280;
		iHeight = 720;
		iFlipType = 0;

		iCustomSet = 0;
		iFps = 25;
		strcpy_s(strOutForm, "I420");
	}

	char strVideoName[255];		//	视频设备名称;
	char strVideoID[255];		//	视频设备ID;
	char strAudioName[255];		//	音频设备名称;
	char strAudioID[255];		//	音频设备ID;
	int iWidth;					//	分辨率-x;
	int iHeight;				//	分辨率-y;
	int iFlipType;				//	旋转类型;

	int iCustomSet;				//	自定义设置;
	int iFps;					//	帧数;
	char strOutForm[100];		//	输出格式;
};

class ImageInfo : public SceneInfo
{// 图片源信息;
public:
	ImageInfo()
	{
		iAlpha = 0;
	}

	char strPath[255];			// 图片路径;
	int iAlpha;					// 图片不透明度;
};

// 视频流参数;
class EVStreamerParameter
{
public:
	EVStreamerParameter()
	{
		frameRate = 15;
		maxVideoBitrate = 500;
		videoResolutionWidth = 1920;
		videoResolutionHeight = 1080;
		iFrameInterval = 75;
	}

	int frameRate;					// 编码帧率fps，默认15
	int maxVideoBitrate;			// 视频初始化码率，默认500kbps
	int videoResolutionWidth;		// 视频编码分辨率，默认为1280。可定义为枚举或常量(0-360x640, 1-540x960, 2-720x1280)
	int videoResolutionHeight;		// 视频编码分辨率，默认为720。可定义为枚举或常量(0-360x640, 1-540x960, 2-720x1280)
	int iFrameInterval;				// i帧间隔
};

enum EFlipType
{
	// 翻转类型;
	FLIPTYPE_NONE = 0x0000,		// 正常无翻转;
	FLIPTYPE_HOR = 0x0001,		// 水平翻转;
	FLIPTYPE_VER = 0x0002		// 垂直翻转;
};

class DevicesInfo
{
public:
	DevicesInfo()
	{
		strcpy_s(strDevName, "");
		strcpy_s(strDevID, "");
	}
	char strDevName[255];
	char strDevID[255];
};