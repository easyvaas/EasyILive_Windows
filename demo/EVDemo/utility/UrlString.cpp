#include "StdAfx.h"
#include "UrlString.h"


CUrlString::CUrlString()
{
	m_iParaCount = 0;
}


CUrlString::~CUrlString()
{
}

CString CUrlString::GetUrlString()
{
	return m_strUrl;
}

CString CUrlString::GetUrlParam()
{
	return m_strUrlParam;
}

void CUrlString::SetUrl(CString strUrl)
{
	m_strUrl.Empty();
	m_strUrl.Append(strUrl);
}

void CUrlString::AddPara(CString strKey, int iValue)
{
	CString strValue;
	strValue.Format(_T("%d"), iValue);
	AddPara(strKey, strValue);
}

void CUrlString::AddPara(CString strKey, CString strValue)
{
	CString strTemp;
	if (m_iParaCount)
	{
		strTemp = _T("&");
	}

	std::string strUtf8 = TStringTool::TString2UTF8(strValue.GetString());
	CString strEncode = _A2T(TStringTool::UrlEncode(strUtf8)).c_str();
	CString strPara;
	strPara.Format(_T("%s%s=%s"), strTemp, strKey, strEncode);
	//m_strUrl.Append(strPara);
	m_strUrlParam.Append(strPara);
	m_iParaCount++;
}