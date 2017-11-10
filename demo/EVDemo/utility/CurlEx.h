#pragma once
#include "curl/curl.h"
#include "Module/DataInfo/User.h"
#include "utility/JsonValueEx.h"
#include "Encrypt/AESEx.h"
#include "LiveLockEx.h"
#include "UrlString.h"
#include "Openlive/UrlEx/UrlExDefine.h"


struct CURLHeader
{
	std::string strKey;
	std::string strValue;
};
typedef std::vector<CURLHeader> VecURLHeader;
class JsonValueEx;

class CTimeTool
{
public:
	// 获取当前的时间戳(精确到秒);
	static CString GetCurSecond()
	{
		CString strSecond;
		strSecond.Format(_T("%d"), time(NULL));
		return strSecond;
	}

	// 获取当前的时间戳(精确到毫秒);
	static CString GetCurMilliSecond()
	{
		SYSTEMTIME st;
		GetLocalTime(&st);
		CString strMilliSecond;
		strMilliSecond.Format(_T("%s%03d"), GetCurSecond(), st.wMilliseconds);
		return strMilliSecond;
	}
};

class CCurlEx
{
public:
	CCurlEx();
	~CCurlEx();

public:
	static CCurlEx* GetInstance();
	static void ReleaseInstance();

	CString	GetErrorInfo();

public:
	// 网络请求;
	//bool PostData(OUT std::string& strResult, IN CString strUrl);
	//bool PostData(OUT JsonValueEx&  jsResult, IN CString strUrl, IN bool VerifyRes = true);
	bool HTTP_RequestData_Get(OUT std::string& strResult, IN CString strUrl);
	bool HTTP_RequestData_Get(OUT JsonValueEx& jsResult, IN CString strUrl, IN bool VerifyRes = true);

	bool HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT std::string& strResult);
	bool HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT JsonValueEx& jsReult);

	// 上传封面头像;
	bool UploadThumbPic(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath);
	// 文件下载;
	bool DownLoadFile(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath);

	bool LiveStartLog();
	// 重连:
	bool LiveReConnectLog();
	// 续播

	// 获取变更rtmp地址;
	bool LiveGetRtmpUrl(OUT JsonValueEx& jsResult, IN CString strRequestUrl, IN CString strSession, IN CString strVid);
	// 结束直播:	会话id、vid必填;   
	bool LiveHeartBeat(IN CString strSession, IN CString strVID, IN int iBits, IN int iFps, IN int iEncq, IN int iAudioBuff, IN int iVideoBuff, IN int iTotalVideoPack, IN int iTotalAudioPack, IN int iSpeed, IN int iBandWidth);
	// 结束直播:	会话id、vid必填;   
	bool LiveStop(IN CString strSession, IN CString strVID, CString strReason = _T("normal"));
	// 删除视频:	会话id、vid必填;   
	bool VideoRemove(IN CString strSession, IN CString strVID);

	// 更新直播状态;
	static void LiveUpdateStatus(LiveStatus iStatus);

	// 注销;
	bool UserLogout(IN CString strSession);

	enum UserManagerAction
	{
		USERMANAGER_ADD = 1,
		USERMANAGER_DEL = 2
	};
	// 设置管理员;
	bool UserManagerSet(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strName, IN UserManagerAction iAction);
	// 获取管理员列表;
	bool GetUserManagerList(OUT JsonValueEx& jsResult, IN CString strSession);
	// 是否管理员;
	bool IsUserManager(IN CString strSession, IN CString strName);

	enum ShutUpType
	{
		SHUTUP_ALLOW	= 0,
		SHUTUP_BAN		= 1
	};
	// 直播间禁言、解禁;
	bool VideoShutUp(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strVID, IN CString strName, IN ShutUpType type);

	// 获取聊天附表;
	bool GetChatSched(OUT JsonValueEx& jsResult, IN CString strSession, CString strVID);
	// 获取聊天状态;
	bool GetChatStatus(OUT JsonValueEx& jsResult, IN CString strUrl);
	// ...

	// 获取优化的strDnsUrl;
	bool GetDnsUrl(OUT CString& strDnsUrl, IN CString strOrgUrl);

protected:
	CString GetSysVersion();
	static size_t HttpCallback(void *pStr, size_t iSize, size_t iCount, void *pOutStr);

	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

	// 获取前置分辨率;
	CString GetFrontCarmes();

private:
	static CLiveLockEx		m_lock;
	static CCurlEx*			m_pInstance;

	CString			m_strError;					// 错误内容;
};

#define AflGetCurlEx				CCurlEx::GetInstance
#define AflCurlDownLoadFile			AflGetCurlEx()->DownLoadFile