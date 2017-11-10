#include "stdafx.h"
#include "UrlEx/CurlEx.h"
#include "Tool/FileEx.h"
#include "Module/DataInfo/UserManage.h"
#include "Main.h"
#include <wininet.h>
#include "Encrypt/Convert.h"
#include "Module/DataInfo/LiveError.h"

#define TIME_CURL_TIMEOUT	(6)							// ��ʱʱ������Ϊ6��;
#define EASYLIVE_KEY		TString(_T("ELive"))			

typedef CURL *(*ex_curl_easy_init)(void);
typedef CURLcode(*ex_curl_easy_setopt)(CURL *curl, CURLoption option, ...);
typedef CURLFORMcode (*ex_curl_formadd)(curl_httppost **httppost, curl_httppost **last_post, ...);
typedef CURLcode(*ex_curl_easy_perform)(CURL *curl);
typedef void(*ex_curl_easy_cleanup)(CURL *curl);
typedef const char *(*ex_curl_easy_strerror)(CURLcode);
typedef CURLcode(*ex_curl_easy_getinfo)(CURL *curl, CURLINFO info, ...);
typedef void(*ex_curl_slist_free_all)(struct curl_slist *);
typedef curl_slist *(*ex_curl_slist_append)(struct curl_slist *, const char *);
typedef char *(*ex_curl_easy_escape)(CURL *handle,
    const char *string,
    int length);

typedef CURLcode (*ex_curl_global_init)(long flags);

HINSTANCE hCurlDll = NULL;
ex_curl_global_init ex_pcurl_global_init = NULL;
ex_curl_easy_init ex_pcurl_easy_init = NULL;
ex_curl_easy_setopt ex_pcurl_easy_setopt = NULL;
ex_curl_formadd ex_pcurl_formadd = NULL;
ex_curl_easy_perform ex_pcurl_easy_perform = NULL;
ex_curl_easy_cleanup ex_pcurl_easy_cleanup = NULL;
ex_curl_easy_strerror ex_pcurl_easy_strerror = NULL;
ex_curl_easy_getinfo ex_pcurl_easy_getinfo = NULL;
ex_curl_slist_free_all ex_pcurl_slist_free_all = NULL;
ex_curl_slist_append ex_pcurl_slist_append = NULL;
ex_curl_easy_escape ex_pcurl_easy_escape = NULL;

CCurlEx* CCurlEx::m_pInstance = NULL;
CLiveLockEx CCurlEx::m_lock;

CCurlEx::CCurlEx()
{
	hCurlDll = LoadLibrary(_T("libcurl.dll"));
	if (NULL != hCurlDll)
	{
		ex_pcurl_global_init = (ex_curl_global_init)GetProcAddress(hCurlDll, "curl_global_init");
		ex_pcurl_easy_init = (ex_curl_easy_init)GetProcAddress(hCurlDll, "curl_easy_init");
		ex_pcurl_easy_setopt = (ex_curl_easy_setopt)GetProcAddress(hCurlDll, "curl_easy_setopt");
		ex_pcurl_formadd = (ex_curl_formadd)GetProcAddress(hCurlDll, "curl_formadd");
		ex_pcurl_easy_perform = (ex_curl_easy_perform)GetProcAddress(hCurlDll, "curl_easy_perform");
		ex_pcurl_easy_cleanup = (ex_curl_easy_cleanup)GetProcAddress(hCurlDll, "curl_easy_cleanup");
		ex_pcurl_easy_strerror = (ex_curl_easy_strerror)GetProcAddress(hCurlDll, "curl_easy_strerror");
		ex_pcurl_easy_getinfo = (ex_curl_easy_getinfo)GetProcAddress(hCurlDll, "curl_easy_getinfo");
		ex_pcurl_slist_free_all = (ex_curl_slist_free_all)GetProcAddress(hCurlDll, "curl_slist_free_all");
		ex_pcurl_slist_append = (ex_curl_slist_append)GetProcAddress(hCurlDll, "curl_slist_append");
        ex_pcurl_easy_escape = (ex_curl_easy_escape)GetProcAddress(hCurlDll, "curl_easy_escape");
		ex_pcurl_global_init(CURL_GLOBAL_ALL);
	}
	else
	{
		DWORD dwErr = GetLastError();
		CString strError;
		strError.Format(_T("����������ļ�ʧ�ܣ�����ԭ��%d"), dwErr);
		LOG_FATAL(strError);
	}
}

CCurlEx::~CCurlEx()
{
	if (NULL != hCurlDll)
	{
		OSFreeLibrary(hCurlDll);
	}
}

CCurlEx* CCurlEx::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_lock.Lock();
		if (NULL == m_pInstance)
		{
			m_pInstance = new CCurlEx();
		}
		m_lock.UnLock();
	}
	return m_pInstance;
}

void CCurlEx::ReleaseInstance()
{
	if (NULL != m_pInstance)
	{
		m_lock.Lock();
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
		m_lock.UnLock();
	}
}

CString CCurlEx::GetErrorInfo()
{
	return m_strError;
}

size_t CCurlEx::HttpCallback(void *pStr, size_t iSize, size_t iCount, void *pOutStr)
{
	if (NULL != pOutStr)
	{
		std::string &str = *(std::string *)pOutStr;
		str.append((const char *)pStr, iSize * iCount);
	}
	return iSize * iCount;
}

bool CCurlEx::HTTP_RequestData_Get(OUT std::string& strResult, IN CString strUrl)
{
	CString strInfo;
	strInfo.Format(_T("Enter CCurlEx::HTTP_RequestData_Get: %s"), strUrl);
	LOG_TRACE(strInfo);

	CURL* pCurl = NULL;
	m_strError.Empty();
	try
	{
		// ���libcurl;
		if (NULL == ex_pcurl_easy_init)
		{
			m_strError = AflGetLiveError(LIVEERROR_CURLFILE_LOSE).c_str();
			throw false;
		}
		else
		{
			pCurl = ex_pcurl_easy_init();
		}

	

		std::string strAnsi = CT2CA(strUrl);
		std::string strUtf8 = TStringTool::AnsiToUtf8(strAnsi.c_str(), strAnsi.length());
		// ����Ŀ���ַ;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_URL, strUtf8.c_str());
		// ֧��zip�ļ�����;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_ACCEPT_ENCODING, "gzip");
		// ����UserAgent; // del by sbd for test

// 		ex_pcurl_easy_setopt(pCurl, CURLOPT_USERAGENT, O_UA);
		// ���ó�ʱʱ��;
		//ex_pcurl_easy_setopt(pCurl, CURLOPT_TIMEOUT, TIME_CURL_TIMEOUT);
		// ������֤;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0);
		ex_pcurl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, FALSE);
 
		// �����ļ���ȡ�Ļص�����;
		strResult.clear();
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpCallback);
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &strResult);

		curl_slist* pHeadList = NULL;
		pHeadList = ex_pcurl_slist_append(pHeadList, "Content-Type:application/x-www-form-urlencoded;charset=UTF-8");
		ex_pcurl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeadList);

		//if (bKeepAlive)
		//{
		//	// ������;
		//	pHeadList = ex_pcurl_slist_append(pHeadList, "Connection: keep-alive");
		//	ex_pcurl_easy_setopt(m_pCurl, CURLOPT_HTTPHEADER, pHeadList);
		//}

		// ��ʼ����;
		CURLcode res = ex_pcurl_easy_perform(pCurl);

		if (NULL != pHeadList)
		{
			ex_pcurl_slist_free_all(pHeadList);
		}
		if (NULL != pCurl)
		{
			ex_pcurl_easy_cleanup(pCurl);
		}
		if (res != CURLE_OK)
		{
			m_strError = AflGetLiveError(res).c_str();
			throw false;
		}
	}
	catch (...)
	{
		if (m_strError.IsEmpty())
		{
			m_strError.Format(_T("%s �������δ֪����"), strUrl);
		}
		CString strError;
		strError.Format( _T("����ԭ��: %s, ����URL: %s, ��������: %s \n"), m_strError, strUrl, _A2T(strResult).c_str() );
		LOG_ERROR(strError);
		return false;
	}

	strInfo.Format(_T("Leave CCurlEx::HTTP_RequestData_Get: URL(%s), Result(%s)"), strUrl, _A2T(strResult).c_str() );
	LOG_TRACE(strInfo);
	return true;
}

bool CCurlEx::HTTP_RequestData_Get(OUT JsonValueEx& jsResult, IN CString strUrl, IN bool VerifyRes/* = true*/)
{
	std::string strResult;
	if (!HTTP_RequestData_Get(strResult, strUrl))
	{
		return false;
	}

	jsResult.clear();

	if (strResult.size() == 0u)
	{
		return false;
	}

	try
	{
		// ��������;
		Json::Reader reader;
		if (strUrl.Find(_T("pay/openredpack")) > -1)
		{
			std::string strAnsi = TStringTool::Utf8ToAnsi(strResult.c_str(), strResult.size());
			if (!reader.parse(strAnsi.c_str(), jsResult))
			{
				m_strError = AflGetLiveError(LIVEERROR_PARSE).c_str();
				throw false;
			}
		}
		else
		{
			if (!reader.parse(strResult.c_str(), jsResult))
			{
				m_strError = AflGetLiveError(LIVEERROR_PARSE).c_str();
				throw false;
			}
		}

		if (!VerifyRes)
		{
			// �еķ������ݲ���retval ��������֤;
			return true;
		}

		std::string strRetVal = jsResult["retval"].GetString();
		if (E_OK != strRetVal)
		{
			m_strError = jsResult["reterr"].GetTString().c_str();

			if (E_SESSION == strRetVal)
			{
				// ��strRetValΪE_SESSION��������˺�ע��;
				m_strError = _T("");

				if (hwndMain)
				{
					// �ر�Session���;
					::PostMessage(hwndMain, WM_LOGIN_OUT, NULL, NULL);
				}
			}
			else if (E_VIDEO_ALREADY_STOPPED == strRetVal)
			{
				m_strError = _T("ֱ����ֹͣ��");
				// ����true�ɣ������޷������߹ر�����;
				return true;
			}
			else if (E_VIDEO_NOT_EXISTS == strRetVal)
			{
				m_strError = _T("ֱ�������ڡ�");
				// ����true�ɣ������޷������߹ر�����;
				return true;
			}
			else if (E_MAX_USER_MANAGER == strRetVal)
			{
				m_strError = _T("����Ա�Ѵ����ޡ�");
			}
			else if (E_SERVER == strRetVal)
			{
				m_strError = _T("����������ʧ�ܣ�");
			}
			throw false;
		}
	}
	catch (...)
	{
		CString strError;
		strError.Format(_T("����ԭ��: %s, ����URL: %s, ��������: %s \n"), m_strError, strUrl, CA2CT(strResult.c_str()) );
		LOG_ERROR(strError + _T("\n"));
		return false;
	}

	return true;
}

bool CCurlEx::HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT std::string& strResult)
{
	//for test
// 	strPostUrl = _T("https://dev.easyvaas.com/test/kkuser/client/login/login");
// 	strPostData = _T("phone=18811742364&password=111111");
	CString strInfo;
	strInfo.Format(_T("Enter CCurlEx::HTTP_RequestData_Post: %s"), strPostUrl);
	LOG_TRACE(strInfo);

	CURL* pCurl = NULL;
	m_strError.Empty();
	try
	{
		// ���libcurl;
		if (NULL == ex_pcurl_easy_init)
		{
			m_strError = AflGetLiveError(LIVEERROR_CURLFILE_LOSE).c_str();
			throw false;
		}
		else
		{
			pCurl = ex_pcurl_easy_init();
		}

		std::string strAnsi = CT2CA(strPostUrl+strMethod);
		std::string strAnsiPostData = CT2CA(strPostData);

		// ������֤;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYHOST, 0);
		ex_pcurl_easy_setopt(pCurl, CURLOPT_SSL_VERIFYPEER, FALSE);

		// �����ļ���ȡ�Ļص�����;
		strResult.clear();
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpCallback);
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &strResult);

		
		ex_pcurl_easy_setopt(pCurl, CURLOPT_POST, 1);                 //����libcurl���͵�Э��  
		ex_pcurl_easy_setopt(pCurl, CURLOPT_URL, strAnsi.c_str());
		ex_pcurl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, strAnsiPostData.c_str());   //����post���ԣ�ʹ��&����������������һ��  

		// ��ʼ����;
		CURLcode res = ex_pcurl_easy_perform(pCurl);
		
// 		if (NULL != pHeadList)
// 		{
// 			ex_pcurl_slist_free_all(pHeadList);
// 		}
		if (NULL != pCurl)
		{
			ex_pcurl_easy_cleanup(pCurl);
		}
		if (res != CURLE_OK)
		{
			m_strError = AflGetLiveError(res).c_str();
			throw false;
		}
	}
	catch (...)
	{
		if (m_strError.IsEmpty())
		{
			m_strError.Format(_T("%s �������δ֪����"), strPostUrl);
		}
		CString strError;
		strError.Format(_T("����ԭ��: %s, ����URL: %s, ��������: %s \n"), m_strError, strPostUrl, _A2T(strResult).c_str());
		LOG_ERROR(strError);
		return false;
	}

	strInfo.Format(_T("Leave CCurlEx::HTTP_RequestData_Post: URL(%s), Result(%s)"), strPostUrl, _A2T(strResult).c_str());
	LOG_TRACE(strInfo);
	return true;
}

bool CCurlEx::HTTP_RequestData_Post(IN CString strPostUrl, IN CString strMethod, IN CString strPostData, OUT JsonValueEx& jsResult)
{
	std::string strResult = "";
	if (!HTTP_RequestData_Post(strPostUrl, strMethod, strPostData, strResult))
	{
		return false;
	}

	jsResult.clear();

	if (strResult.size() == 0u)
	{
		m_strError = AflGetLiveError(LIVEERROR_EMPTY_RESULT).c_str();
		return false;
	}

	try
	{
		// ��������;
		Json::Reader reader;
		if (strPostUrl.Find(_T("pay/openredpack")) > -1)
		{
			std::string strAnsi = TStringTool::Utf8ToAnsi(strResult.c_str(), strResult.size());
			if (!reader.parse(strAnsi.c_str(), jsResult))
			{
				m_strError = AflGetLiveError(LIVEERROR_PARSE).c_str();
				throw false;
			}
		}
		else
		{
			if (!reader.parse(strResult.c_str(), jsResult))
			{
				m_strError = AflGetLiveError(LIVEERROR_PARSE).c_str();
				throw false;
			}
		}

// 		if (!VerifyRes)
// 		{
// 			// �еķ������ݲ���retval ��������֤;
// 			return true;
// 		}

		std::string strRetVal = jsResult["retval"].GetString();
		if (E_OK != strRetVal)
		{
			m_strError = jsResult["reterr"].GetTString().c_str();

			if (E_SESSION == strRetVal)
			{
				// ��strRetValΪE_SESSION��������˺�ע��;
				m_strError = _T("");

				if (hwndMain)
				{
					// �ر�Session���;
					::PostMessage(hwndMain, WM_LOGIN_OUT, NULL, NULL);
				}
			}
			else if (E_VIDEO_ALREADY_STOPPED == strRetVal)
			{
				m_strError = _T("ֱ����ֹͣ��");
				// ����true�ɣ������޷������߹ر�����;
				return true;
			}
			else if (E_VIDEO_NOT_EXISTS == strRetVal)
			{
				m_strError = _T("ֱ�������ڡ�");
				// ����true�ɣ������޷������߹ر�����;
				return true;
			}
			else if (E_MAX_USER_MANAGER == strRetVal)
			{
				m_strError = _T("����Ա�Ѵ����ޡ�");
			}
			else if (E_SERVER == strRetVal)
			{
				m_strError = _T("����������ʧ�ܣ�");
			}
			throw false;
		}
	}
	catch (...)
	{
		CString strError;
		strError.Format(_T("����ԭ��: %s, ����URL: %s, ��������: %s \n"), m_strError, strPostUrl, CA2CT(strResult.c_str()));
		LOG_ERROR(strError + _T("\n"));
		return false;
	}
	return true;
}

/*******************************************************************************
* ����������	��ȡWindows�汾��Ϣ
* ���������
* ���������
* �� �� ֵ��
* ����˵����
* �޸�����		�޸���			�޸�����
* ------------------------------------------------------------------------------
* 2016-04-05	�ﳬ	      	����
*******************************************************************************/
CString CCurlEx::GetSysVersion()
{
	OSVERSIONINFOEX osVersion;
	osVersion.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
	if (!GetVersionEx((LPOSVERSIONINFO)&osVersion))
	{
		return _T("GetVersionError");
	}

	if (VER_PLATFORM_WIN32_NT == osVersion.dwPlatformId)
	{
		switch (osVersion.dwMajorVersion)
		{
		case 5:
		{
				  switch (osVersion.dwMinorVersion)
				  {
				  case 0:
				  {
							return _T("2000");
				  }
					  break;
				  case 1:
				  {
							return _T("XP");
				  }
					  break;
				  default:
					  break;
				  }
		}
			break;
		case 6:
		{
				  switch (osVersion.dwMinorVersion)
				  {
				  case 0:
				  {
							return _T("Vista");
				  }
					  break;
				  case 1:
				  {
							return _T("win7");
				  }
					  break;
				  case 2:
				  {
							return _T("win8");
				  }
					  break;
				  case 3:
				  {
							return _T("win8.1");
				  }
					  break;
				  default:
					  break;
				  }
				  break;
		}
		default:
			break;
		}
	}

	return _T("");
}

bool CCurlEx::LiveGetRtmpUrl(OUT JsonValueEx& jsResult, IN CString strRequestUrl, IN CString strSession, IN CString strVid)
{
	CUrlString strUrlString;
	CString strUrl = strRequestUrl;
	strUrl.Append(_T("?"));
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);
	strUrlString.AddPara(_T("vid"), strVid);

	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}

	return true;
}

bool CCurlEx::LiveStartLog()
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/yizhibo?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetLiveLogURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("from"), _T("pc"));
	strUrlString.AddPara(_T("session"), AflGetUserInfo()->strSession);
	strUrlString.AddPara(_T("name"), AflGetUserInfo()->strName);
	strUrlString.AddPara(_T("pts"), CTimeTool::GetCurMilliSecond());
	strUrlString.AddPara(_T("module"), _T("live"));
	strUrlString.AddPara(_T("action"), _T("livestart"));
	strUrlString.AddPara(_T("vid"), AflGetLiveInfo()->strVID);
	strUrlString.AddPara(_T("ostype"), EASYLIVE_WINDOWS);
	strUrlString.AddPara(_T("osver"), CCurlEx::GetSysVersion());
	strUrlString.AddPara(_T("puburl"), AflGetLiveInfo()->strLiveUrl);
	strUrlString.AddPara(_T("frontcamres"), GetFrontCarmes());
	strUrlString.AddPara(_T("backcamres"), 0);

	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strUrlString.GetUrlString();
	cmdItem.cmdInfo.hCallBackWnd = NULL;
	cmdItem.pFunc = NULL;
	AflGetCmdManager()->AddCmd(cmdItem);

	return true;
}

bool CCurlEx::LiveReConnectLog()
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/yizhibo?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetLiveLogURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("from"), _T("pc"));
	strUrlString.AddPara(_T("session"), AflGetUserInfo()->strSession);
	strUrlString.AddPara(_T("name"), AflGetUserInfo()->strName);
	strUrlString.AddPara(_T("pts"), CTimeTool::GetCurMilliSecond());
	strUrlString.AddPara(_T("module"), _T("live"));
	strUrlString.AddPara(_T("action"), _T("reconnect"));
	strUrlString.AddPara(_T("vid"), AflGetLiveInfo()->strVID);
	strUrlString.AddPara(_T("ostype"), EASYLIVE_WINDOWS);
	strUrlString.AddPara(_T("osver"), CCurlEx::GetSysVersion());
	strUrlString.AddPara(_T("puburl"), AflGetLiveInfo()->strLiveUrl);
	strUrlString.AddPara(_T("frontcamres"), GetFrontCarmes());
	strUrlString.AddPara(_T("backcamres"), 0);

	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strUrlString.GetUrlString();
	cmdItem.cmdInfo.hCallBackWnd = NULL;
	cmdItem.pFunc = NULL;
	AflGetCmdManager()->AddCmd(cmdItem);

	return true;
}

//	action = heartbeat
//	vid = [vid]
//	bitrate = [bitrate]					//���õı������ʣ���λ��kbps
//	fps = [fps]							//�ɼ�����Ƶ֡��
//	encq = [encq]						//r
//	audiobufferuse = [used - max]		//��Ƶ��������ʹ��-���
//	videobufferuse = [used - max]		//��Ƶ��������ʹ��-���
//	totalvideopack = [totalvideopack]	//�Ѿ��ɼ�������Ƶ֡����
//	totalaudiopack = [totalaudiopack]	//�Ѿ��ɼ�������Ƶ֡����
//	speed = [speed]						//��λ��kb����λ��ЧС��
//	bandwidth = [bandwidth]				//��λ��kb����λ��ЧС��
bool CCurlEx::LiveHeartBeat(IN CString strSession, IN CString strVID, IN int iBits, IN int iFps, IN int iEncq, IN int iAudioBuff, IN int iVideoBuff, IN int iTotalVideoPack, IN int iTotalAudioPack, IN int iSpeed, IN int iBandWidth)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/yizhibo?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetLiveLogURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("from"), _T("pc"));
	strUrlString.AddPara(_T("session"), strSession);
	strUrlString.AddPara(_T("name"), AflGetUserInfo()->strName);
	strUrlString.AddPara(_T("pts"), CTimeTool::GetCurMilliSecond());
	strUrlString.AddPara(_T("module"), _T("live"));
	strUrlString.AddPara(_T("action"), _T("heartbeat"));
	strUrlString.AddPara(_T("vid"), strVID);
	strUrlString.AddPara(_T("bitrate"), iBits);
	strUrlString.AddPara(_T("fps"), iFps);
	strUrlString.AddPara(_T("encq"), iEncq);
	strUrlString.AddPara(_T("audiobufferuse"), iAudioBuff);
	strUrlString.AddPara(_T("videobufferuse"), iVideoBuff);
	strUrlString.AddPara(_T("totalvideopack"), iTotalVideoPack);
	strUrlString.AddPara(_T("totalaudiopack"), iTotalAudioPack);
	strUrlString.AddPara(_T("speed"), iSpeed);
	strUrlString.AddPara(_T("bandwidth"), iBandWidth);

	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strUrlString.GetUrlString();
	cmdItem.cmdInfo.hCallBackWnd = NULL;
	cmdItem.pFunc = NULL;
	AflGetCmdManager()->AddCmd(cmdItem);

	return true;
}

bool CCurlEx::LiveStop(IN CString strSession, CString strVID, CString strReason)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/livestop?sessionid=%s&vid=%s"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL(), strSession.GetString(), strVID.GetBuffer());
// 	strUrlString.SetUrl(strUrl);
// 	strUrlString.AddPara(_T("sessionid"), strSession);
// 	strUrlString.AddPara(_T("vid"), strVID);

	JsonValueEx jsResult;
	if (!HTTP_RequestData_Get(jsResult, strUrl))
	{
		return false;
	}

	// ֱ��ֹͣ��־�ϴ�;
	CUser* pUserInfo = AflGetUserInfo();
	if (NULL != pUserInfo)
	{
		CUrlString strUrlLogString;
		CString strUrlLog;
		strUrlLog.Format(_T("%s/yizhibo?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetLiveLogURL());
		strUrlLogString.SetUrl(strUrlLog);
		strUrlLogString.AddPara(_T("from"), _T("pc"));
		strUrlLogString.AddPara(_T("session"), AflGetUserInfo()->strSession);
		strUrlLogString.AddPara(_T("name"), pUserInfo->strName);
		strUrlLogString.AddPara(_T("pts"), CTimeTool::GetCurMilliSecond());
		strUrlLogString.AddPara(_T("module"), _T("live"));
		strUrlLogString.AddPara(_T("action"), _T("livestop"));
		strUrlLogString.AddPara(_T("vid"), AflGetLiveInfo()->strVID);
		strUrlLogString.AddPara(_T("reason"), strReason);

		std::string strLogResult;
		if (!HTTP_RequestData_Get(strLogResult, strUrlLogString.GetUrlString()))
		{
			// Log�ϴ�����ʧ�ܲ���Ӱ��ֱ������ �ʴ˴�����true;
			return true;
		}
	}

	return true;
}

bool CCurlEx::UserLogout(IN CString strSession)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/userlogout?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);

	// ע��ʱ�������ж�Json ����SessionʧЧ����ע��ʱ�������ѭ��;
	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strUrlString.GetUrlString();
	cmdItem.cmdInfo.hCallBackWnd = NULL;
	cmdItem.pFunc = NULL;
	AflGetCmdManager()->AddCmd(cmdItem);

	return true;
}

bool CCurlEx::GetChatSched(OUT JsonValueEx& jsResult, IN CString strSession, CString strVID)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/getchatsched?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);
	strUrlString.AddPara(_T("vid"), strVID);

	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}
	return true;
}

// ��ȡ����״̬;
bool CCurlEx::GetChatStatus(OUT JsonValueEx& jsResult, IN CString strUrl)
{
	if (!HTTP_RequestData_Get(jsResult, strUrl, false))
	{
		return false;
	}
	return true;
}

bool CCurlEx::GetDnsUrl(OUT CString& strDnsUrl, IN CString strOrgUrl)
{
	CURL *pCurl = NULL;
	m_strError.Empty();
	try
	{
		if (strOrgUrl.Find(_T("://wspush")) < 0)
		{
			// �Ҳ�����://wspush���������Ż�����Ĭ��������ַ��Ϊ�Ż���ĵ�ַ;
			strDnsUrl = strOrgUrl;
			return true;
		}

		// ���libcurl;
		if (NULL == ex_pcurl_easy_init)
		{
			m_strError = AflGetLiveError(LIVEERROR_CURLFILE_LOSE).c_str();
			throw false;
		}
		else
		{
			pCurl = ex_pcurl_easy_init();
		}

		// ����Ŀ���ַ;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_URL, "http://sdk.wscdns.com");

		// �����ļ���ȡ�Ļص�����;
		std::string strResult;
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, HttpCallback);
		ex_pcurl_easy_setopt(pCurl, CURLOPT_WRITEDATA, &strResult);

		curl_slist* pHeaderList = NULL;
		if (NULL == pHeaderList)
		{
			std::string strHeader1 = "WS_URL: ";
			strHeader1.append(CT2CA(strOrgUrl));
			std::string strHeader2 = "WS_RETIP_NUM: 1";
			std::string strHeader3 = "WS_URL_TYPE: 3";

			pHeaderList = ex_pcurl_slist_append(pHeaderList, strHeader1.c_str());
			pHeaderList = ex_pcurl_slist_append(pHeaderList, strHeader2.c_str());
			pHeaderList = ex_pcurl_slist_append(pHeaderList, strHeader3.c_str());
			ex_pcurl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeaderList);
		}

		// ��ʼ����;
		CURLcode res = ex_pcurl_easy_perform(pCurl);
		if (NULL != pHeaderList)
		{
			ex_pcurl_slist_free_all(pHeaderList);
		}
		if (res != CURLE_OK)
		{
			m_strError = CA2CT(ex_pcurl_easy_strerror(res));
			throw false;
		}

		// ȥ�����Ļ��з�\n;
		strDnsUrl = CA2CT(strResult.c_str());
		if (strDnsUrl.Find(_T("rtmp")) < 0 )
		{
			// �Ҳ���rtmp��ת��ʧ��;
			throw false;
		}
		strDnsUrl = strDnsUrl.Left(strDnsUrl.GetLength() - 1);
	}
	catch (...)
	{
		if (m_strError.IsEmpty())
		{
			m_strError.Format(_T("%s DNSת���������δ֪����"), strOrgUrl);
		}
		if (NULL != pCurl)
		{
			ex_pcurl_easy_cleanup(pCurl);
		}
		LOG_ERROR(m_strError);

		return false;
	}

	if (NULL != pCurl)
	{
		ex_pcurl_easy_cleanup(pCurl);
	}
	return true;
}

bool CCurlEx::UploadThumbPic(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath)
{
	string strPostUrl = _T2A(strUrl);
    string strPath = _T2A(strLocalPath); //  "E:\\11.png";
    std::string strFileExt = strPath.substr(strPath.size() - 3);
	CURL *curl;
	CURLcode res;
	FILE* pFile;
	struct curl_slist *http_header = NULL;

	if ((pFile = fopen(strPath.c_str(), "rb")) == NULL)
	{
		char chErr[1024];
		sprintf_s(chErr, "%s ��ʧ�ܣ�\n", _T2A(strLocalPath));
		LOG_ERROR(chErr);
		m_strError = _A2T(chErr).c_str();
		return false;
	}

    fseek(pFile, 0, SEEK_END);		/// ���ļ�ָ���ƶ��ļ���β;
    long fclen = ftell(pFile);
    char * fc = new char[fclen];
    fseek(pFile, 0, SEEK_SET);
    fread(fc, 1, fclen, pFile);
    fclose(pFile);

	curl = ex_pcurl_easy_init();
    // ����Ŀ���ַ;
    std::string strAnsi = CT2CA(strUrl);
    std::string strUtf8 = TStringTool::AnsiToUtf8(strAnsi.c_str(), strAnsi.length());
    ex_pcurl_easy_setopt(curl, CURLOPT_URL, strUtf8.c_str());
    std::string data("req=plain");
    data.append("&file=");
    // char *efc = ex_pcurl_easy_escape(curl, fc, fclen);
    // data.append(efc);
    

    // curl_easy_setopt(curl, CURLOPT_URL, PURGE_URL);
    
    // ex_pcurl_easy_setopt(curl, CURLOPT_POST, 1L);
    // ex_pcurl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    // ex_pcurl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());

    curl_httppost *post = NULL;
    curl_httppost *last = NULL;
    // ex_pcurl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILECONTENT, strPath.c_str(), CURLFORM_END);
    
    std::string strUrlFileName = "uname.";
    strUrlFileName += strFileExt;
    std::string strContentType = "image/";
    strContentType += strFileExt;
    ex_pcurl_formadd(&post, &last,
        CURLFORM_COPYNAME, "file",
        CURLFORM_BUFFER, strUrlFileName.c_str(), // "unnamed.png",
        CURLFORM_BUFFERPTR, fc,
        CURLFORM_BUFFERLENGTH, fclen,
        CURLFORM_CONTENTTYPE, strContentType.c_str(), // "image/png",
        CURLFORM_END);
    ex_pcurl_easy_setopt(curl, CURLOPT_HTTPPOST, post);

    /*
	ex_pcurl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
	ex_pcurl_easy_setopt(curl, CURLOPT_WRITEDATA, pFile);

	struct curl_httppost *formpost = 0;
	struct curl_httppost *lastptr = 0;
	//ex_pcurl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "reqformat", CURLFORM_PTRCONTENTS, "plain", CURLFORM_END);
	ex_pcurl_formadd(&formpost, &lastptr, CURLFORM_PTRNAME, "file", CURLFORM_FILE, strPath.c_str(), CURLFORM_END);
	ex_pcurl_easy_setopt(curl, CURLOPT_URL, strPostUrl.c_str());
	ex_pcurl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    */

    std::string strResult;
    strResult.clear();
    ex_pcurl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpCallback);
    ex_pcurl_easy_setopt(curl, CURLOPT_WRITEDATA, &strResult);

	res = ex_pcurl_easy_perform(curl);
	ex_pcurl_easy_cleanup(curl);

	//ex_pcurl_global_init(CURL_GLOBAL_WIN32);
	//CURL*curl = ex_pcurl_easy_init();

	//// ʹ��multi-parts form post  
	// ex_pcurl_easy_setopt(curl, CURLOPT_URL, strPostUrl.c_str());
	
	// �ı�����  
	//ex_pcurl_formadd(&post, &last, CURLFORM_COPYNAME, "name", CURLFORM_COPYCONTENTS, "JGood", CURLFORM_END); 
	//ex_pcurl_formadd(&post, &last, CURLFORM_COPYNAME, "address", CURLFORM_COPYCONTENTS, "HangZhou", CURLFORM_END);
	// �ı��ļ��е�����  
	// ex_pcurl_formadd(&post, &last, CURLFORM_COPYNAME, "file", CURLFORM_FILECONTENT, strPath.c_str(), CURLFORM_END);
    // ex_pcurl_easy_setopt(curl, CURLOPT_HTTPPOST, post);
	//CURLcode res = ex_pcurl_easy_perform(easy_handle);
	////ex_pcurl_formfree(post);
	//ex_pcurl_easy_cleanup(easy_handle);

    delete[] fc;

	return true;
}

size_t CCurlEx::write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	FILE *fptr = (FILE*)userp;
	fwrite(buffer, size, nmemb, fptr);

	return size * nmemb;
}

bool CCurlEx::DownLoadFile(IN LPCWSTR strUrl, OUT LPCWSTR strLocalPath)
{
	std::string strResult;
	if (!HTTP_RequestData_Get(strResult, strUrl))
	{
		return false;
	}

	FILE* fp = NULL;
	fopen_s(&fp, _T2A(strLocalPath).c_str(), "wb+");
	if (NULL == fp)
	{
		return false;
	}
	size_t nWrite = fwrite(strResult.c_str(), sizeof(char), strResult.size(), fp);
	fclose(fp);
	return true;
}

bool CCurlEx::UserManagerSet(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strName, IN UserManagerAction iAction)
{
	CString strAction;
	switch (iAction)
	{
	case USERMANAGER_ADD:
		strAction = _T("add");
		break;
	case USERMANAGER_DEL:
		strAction = _T("del");
		break;
	default:
		break;
	}

	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/usermanagerset?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);
	strUrlString.AddPara(_T("name"), strName);
	strUrlString.AddPara(_T("action"), strAction);

	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}
	return true;
}

// ��ȡ����Ա�б�;
bool CCurlEx::GetUserManagerList(OUT JsonValueEx& jsResult, IN CString strSession)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/usermanagerlist?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);

	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}
	return true;
}

// �Ƿ����Ա;
bool CCurlEx::IsUserManager(IN CString strSession, IN CString strName)
{
	JsonValueEx jsResult;
	if (GetUserManagerList(jsResult, strSession))
	{
		JsonValueEx jsList = jsResult["retinfo"]["users"];
		for (int i = 0; i < jsList.size(); i++)
		{
			TString strTemp = jsList[i]["name"].GetTString();
			if ( 0 == strName.Compare(strTemp.c_str()) )
			{
				return true;
			}
		}
	}
	return false;
}

bool CCurlEx::VideoShutUp(OUT JsonValueEx& jsResult, IN CString strSession, IN CString strVID, IN CString strName, IN ShutUpType type)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/videoshutup?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);
	strUrlString.AddPara(_T("vid"), strVID);
	strUrlString.AddPara(_T("name"), strName);
	strUrlString.AddPara(_T("shutup"), TypeToInt(type));

	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}
	return true;
}

CString CCurlEx::GetFrontCarmes()
{
	if (NULL == AppConfig)
	{
		return _T("");
	}
	int iBaseWidth = AppConfig->GetInt(TEXT("Video"), TEXT("BaseWidth"), BASE_WIDTH);
	int iBaseHeight = AppConfig->GetInt(TEXT("Video"), TEXT("BaseHeight"), BASE_HEIGHT);
	double dScale = AppConfig->GetFloat(TEXT("Video"), TEXT("Downscale"), 1.0);
	CString strRatio;
	strRatio.Format(_T("%d*%d"), TypeToInt(iBaseWidth / dScale), TypeToInt(iBaseHeight / dScale));
	return strRatio;
}

bool CCurlEx::VideoRemove(IN CString strSession, IN CString strVID)
{
	CUrlString strUrlString;
	CString strUrl;
	strUrl.Format(_T("%s/videoremove?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strUrlString.SetUrl(strUrl);
	strUrlString.AddPara(_T("sessionid"), strSession);
	strUrlString.AddPara(_T("vid"), strVID);

	JsonValueEx jsResult;
	if (!HTTP_RequestData_Get(jsResult, strUrlString.GetUrlString()))
	{
		return false;
	}
	return true;
}

void CCurlEx::LiveUpdateStatus(LiveStatus iStatus)
{
	// ����ֱ��״̬ 5��һ��;
	CUrlString strCmd;
	CString strUrl;
	strUrl.Format(_T("%s/liveupdatestatus?"), (dynamic_cast<CZLiveApp*>(AfxGetApp()))->GetAppServerSecureURL());
	strCmd.SetUrl(strUrl);
	strCmd.AddPara(_T("sessionid"), AflGetUserInfo()->strSession);
	strCmd.AddPara(_T("vid"), AflGetLiveInfo()->strVID);
	strCmd.AddPara(_T("status"), (int)(iStatus));
	strCmd.AddPara(_T("device"), _T("windows"));

	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strCmd.GetUrlString();
	cmdItem.pFunc = NULL;
	AflGetCmdManager()->AddCmd(cmdItem);
}