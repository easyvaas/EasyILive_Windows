#include "stdafx.h"
#include "UrlString.h"
#include <sstream>
#include "TString\TStringTool.h"

CUrlString::CUrlString()
{
}

CUrlString::~CUrlString()
{
}

TString CUrlString::GetUrlString()
{
	TString strUrl = m_strUrl;
	int iIndex = 0;
	for (MapPara::iterator it = m_mapParas.begin(); it != m_mapParas.end(); ++it)
	{	
		TString strPara;
		if (iIndex++ > 0)
		{
			strPara.append(_T("&"));
		}
		strPara.append(it->first);
		strPara.append(_T("="));
		strPara.append(it->second);

		strUrl.append(strPara);
	}

	return strUrl;
}

void CUrlString::SetUrl(IN TString strUrl)
{
	m_strUrl = strUrl;
}

void CUrlString::AddPara(IN TString strKey, IN int iValue)
{
	std::wstringstream strStream;
	strStream << iValue;
	TString strValue;
	strStream >> strValue;
	AddPara(strKey, strValue);
}

void CUrlString::AddPara(IN TString strKey, IN TString strValue)
{
	std::string strUtf8 = TStringTool::TString2UTF8(strValue);
	TString strEncodeValue = _A2T(TStringTool::UrlEncode(strUtf8)).c_str();
	m_mapParas[strKey] = strEncodeValue;
}