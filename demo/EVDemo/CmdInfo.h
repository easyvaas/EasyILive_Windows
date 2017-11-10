#pragma once
#include <list>
#include "TString\TString.h"
#include "EVLiveLock.h"
#include "CurlEx.h"
#include "JsonValueEx.h"

class CmdInfo
{
public:
	CmdInfo()
	{
		strCmd = _T("");
		bSecurity = false;
		vecHeader.clear();

		iDelay = 0;
		bSucess = true;
		iErrorCode = 0;
		strError = _T("");
		jsResult.clear();
		hCallBackWnd = NULL;
	}
	CmdInfo(const CmdInfo& other) :strCmd(other.strCmd), bSecurity(other.bSecurity), vecHeader(other.vecHeader), bSucess(other.bSucess), strError(other.strError)
	{
		jsResult = other.jsResult;
		hCallBackWnd = other.hCallBackWnd;
	}

	TString			strCmd;
	bool			bSecurity;
	VecURLHeader	vecHeader;

	int				iDelay;
	bool			bSucess;
	int				iErrorCode;
	TString			strError;
	JsonValueEx		jsResult;
	HWND			hCallBackWnd;		// 回调时给窗口句柄发送消息;
};

typedef void(*Curl_CallBack) (CmdInfo& pCmdInfo);

class CmdItem
{
public:
	CmdItem()
	{

	}
	CmdItem(const CmdItem& other) :cmdInfo(other.cmdInfo), pFunc(other.pFunc)
	{
	}

	CmdInfo			cmdInfo;
	Curl_CallBack	pFunc;
};
typedef std::list<CmdItem> CmdList;		// 命令队列;

// 命令控制;
class CmdManager
{
private:
	CmdManager();
	~CmdManager();

public:
	static CmdManager* GetInstance();
	static void ReleaseInstance();

	void AddCmd(IN TString& strCmd, IN Curl_CallBack pFunc = NULL);
	void AddCmd(IN CmdItem& cmdInfo);

private:
	CmdList		m_listCmd;						// 命令队列;
	DWORD		m_dwThreadID;
	HANDLE		m_hThCmd;						// 线程句柄;
	bool		m_bStop;						// 命令控制;
	CLiveLockEx	m_lockList;						// 命令队列数据锁头;

	void DeInit();

	static DWORD WINAPI Th_Cmd(LPVOID lpParam);

	static CmdManager*	m_pInstance;
	static CLiveLockEx	m_lockInst;
};

#define AflGetCmdManager	CmdManager::GetInstance