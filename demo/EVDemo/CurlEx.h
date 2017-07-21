#pragma once

#include "JsonValueEx.h"

#define EV_ERROR_PARAM		"E_PARAM"

struct CURLHeader
{
	std::string strKey;
	std::string strValue;
};
typedef std::vector<CURLHeader> VecURLHeader;
class JsonValueEx;

class CCurlEx
{
public:
	CCurlEx();
	~CCurlEx();

	TString	GetErrorInfo();

public:
	// 文件下载;
	bool DownLoadFile(IN LPCWSTR strUrl, IN LPCWSTR strLocalPath);

	// 网络请求;
	bool PostData(OUT std::string& strResult, IN TString strUrl, IN bool bSecurity = false, IN VecURLHeader& vecHeader = VecURLHeader());
	bool PostData(OUT JsonValueEx&  jsResult, IN TString strUrl, IN bool bSecurity = false, IN VecURLHeader& vecHeader = VecURLHeader());

protected:
	static size_t HttpCallback(void *pStr, size_t iSize, size_t iCount, void *pOutStr);

private:
	TString					m_strError;		// 错误内容;
};