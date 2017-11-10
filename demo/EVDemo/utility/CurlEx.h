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
	// ��ȡ��ǰ��ʱ���(��ȷ����);
	static CString GetCurSecond()
	{
		CString strSecond;
		strSecond.Format(_T("%d"), time(NULL));
		return strSecond;
	}

	// ��ȡ��ǰ��ʱ���(��ȷ������);
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
	// ��������;
	//bool PostData(OUT std::string& strResult, IN CString strUrl);
	//bool PostData(OUT JsonValueEx&  jsResult, IN CString strUrl, IN bool VerifyRes = true);
	bool HTTP_RequestData_Get(OUT std::string& strResult, IN CString strUrl);
	bool HTTP_RequestData_Get(OUT JsonValueEx& jsResult, IN CString strUrl, IN bool VerifyRes = true);

	bool HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT std::string& strResult);
	bool HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT JsonValueEx& jsReult);

	// �ϴ�����ͷ��;
	bool UploadThumbPic(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath);
	// �ļ�����;
	bool DownLoadFile(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath);

	bool LiveStartLog();
	// ����:
	bool LiveReConnectLog();
	// ����

	// ��ȡ���rtmp��ַ;
	bool LiveGetRtmpUrl(OUT JsonValueEx& jsResult, IN CString strRequestUrl, IN CString strSession, IN CString strVid);
	// ����ֱ��:	�Ựid��vid����;   
	bool LiveHeartBeat(IN CString strSession, IN CString strVID, IN int iBits, IN int iFps, IN int iEncq, IN int iAudioBuff, IN int iVideoBuff, IN int iTotalVideoPack, IN int iTotalAudioPack, IN int iSpeed, IN int iBandWidth);
	// ����ֱ��:	�Ựid��vid����;   
	bool LiveStop(IN CString strSession, IN CString strVID, CString strReason = _T("normal"));
	// ɾ����Ƶ:	�Ựid��vid����;   
	bool VideoRemove(IN CString strSession, IN CString strVID);

	// ����ֱ��״̬;
	static void LiveUpdateStatus(LiveStatus iStatus);

	// ע��;
	bool UserLogout(IN CString strSession);

	enum UserManagerAction
	{
		USERMANAGER_ADD = 1,
		USERMANAGER_DEL = 2
	};
	// ���ù���Ա;
	bool UserManagerSet(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strName, IN UserManagerAction iAction);
	// ��ȡ����Ա�б�;
	bool GetUserManagerList(OUT JsonValueEx& jsResult, IN CString strSession);
	// �Ƿ����Ա;
	bool IsUserManager(IN CString strSession, IN CString strName);

	enum ShutUpType
	{
		SHUTUP_ALLOW	= 0,
		SHUTUP_BAN		= 1
	};
	// ֱ������ԡ����;
	bool VideoShutUp(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strVID, IN CString strName, IN ShutUpType type);

	// ��ȡ���츽��;
	bool GetChatSched(OUT JsonValueEx& jsResult, IN CString strSession, CString strVID);
	// ��ȡ����״̬;
	bool GetChatStatus(OUT JsonValueEx& jsResult, IN CString strUrl);
	// ...

	// ��ȡ�Ż���strDnsUrl;
	bool GetDnsUrl(OUT CString& strDnsUrl, IN CString strOrgUrl);

protected:
	CString GetSysVersion();
	static size_t HttpCallback(void *pStr, size_t iSize, size_t iCount, void *pOutStr);

	static size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp);

	// ��ȡǰ�÷ֱ���;
	CString GetFrontCarmes();

private:
	static CLiveLockEx		m_lock;
	static CCurlEx*			m_pInstance;

	CString			m_strError;					// ��������;
};

#define AflGetCurlEx				CCurlEx::GetInstance
#define AflCurlDownLoadFile			AflGetCurlEx()->DownLoadFile