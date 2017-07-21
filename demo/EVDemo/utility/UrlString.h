#pragma once

class CUrlString
{
public:
	CUrlString();
	~CUrlString();

	CString GetUrlString();
	CString GetUrlParam();

	void SetUrl(CString strUrl);

	void AddPara(CString strKey, int iValue);
	// 最后统一走此接口组装;
	void AddPara(CString strKey, CString strValue);

private:
	CString		m_strUrl;
	int			m_iParaCount;
	CString     m_strUrlParam = _T("");
};