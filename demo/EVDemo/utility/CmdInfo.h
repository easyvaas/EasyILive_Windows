#pragma once
#include <list>
#include "TString\TString.h"
#include "LiveLockEx.h"
#include "UrlEx/CurlEx.h"
#include "utility/JsonValueEx.h"
#include <memory>

class CmdInfo
{
public:
	enum CMD_TYPE
	{
		CMD_TYPE_GETCHANNELS = 0,
		CMD_TYPE_OTHERS
	};

	CmdInfo()
		: cmdType(CMD_TYPE_OTHERS)
		, bScrollUp(false)
	//	, m_hSemaphoreGetList(NULL)
	{
		strCmd = _T("");
		bVerifyRes = true;
		vecHeader.clear();

		iDelay = 0;
		bSucess = true;
		iErrorCode = 0;
		strError = _T("");
		jsResult.clear();
		hCallBackWnd = NULL;

		url_ex_data.strUrl = _T("");
		url_ex_data.request_method = _T("");
		url_ex_data.url_data = _T("");
		url_ex_data.url_method = _T("");
	}
	CmdInfo(const CmdInfo& other) 
		:strCmd(other.strCmd)
		, bVerifyRes(other.bVerifyRes)
		, vecHeader(other.vecHeader)
		, bSucess(other.bSucess)
		, strError(other.strError)
		, cmdType(other.cmdType)
		, bScrollUp(other.bScrollUp)
	//	, m_hSemaphoreGetList(other.m_hSemaphoreGetList)
	{
		jsResult = other.jsResult;
		hCallBackWnd = other.hCallBackWnd;

		url_ex_data.strUrl = other.url_ex_data.strUrl;
		url_ex_data.request_method = other.url_ex_data.request_method;
		url_ex_data.url_data = other.url_ex_data.url_data;
		url_ex_data.url_method = other.url_ex_data.url_method;
	}

	TString			strCmd;
	
	URLEXRequestData url_ex_data;
	bool			bVerifyRes;
	VecURLHeader	vecHeader;

	int				iDelay;
	bool			bSucess;
	int				iErrorCode;
	TString			strError;
	JsonValueEx		jsResult;
	HWND			hCallBackWnd;		// 回调时给窗口句柄发送消息;

	CMD_TYPE        cmdType;  //为了区分处理获取频道列表的命令

	//cmdType为CMD_TYPE_GETCHANNELS时，才有效 true表示滚动条到顶刷新新添加的直播，
	bool bScrollUp;
	//HANDLE          m_hSemaphoreGetList;
};

typedef void(*Curl_CallBack) (CmdInfo& pCmdInfo);

class CmdItem
{
public:
	CmdItem()
	{
		pFunc = NULL;
	}
	CmdItem(const CmdItem& other) :cmdInfo(other.cmdInfo), pFunc(other.pFunc)
	{
	}

	CmdInfo			cmdInfo;
	Curl_CallBack	pFunc;
};

typedef std::shared_ptr<CmdItem> CmdItemPtr;	
typedef std::list<CmdItem> CmdList;				// 命令队列;

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

	void AddCmdGetChannels(IN CmdItem& cmdItem, HANDLE& hSemaphore);
private:
	CmdList		m_listCmd;						// 命令队列;
	DWORD		m_dwThreadID;
	HANDLE		m_hThCmd;						// 线程句柄;
	bool		m_bStop;						// 命令控制;
	CLiveLockEx	m_lockList;						// 命令队列数据锁头;


	//获取直播频道命令队列
	CmdList     m_listCmdGetChannels;
	CLiveLockEx m_lockListGetChannels;
	UINT        m_dwThreadIDGetChannels;
	HANDLE      m_hThreadCmdGetChannels;
	bool        m_bStopGetChannels;

	void DeInit();

	static DWORD WINAPI Th_Cmd(LPVOID lpParam);

	static DWORD WINAPI Th_Cmd_Excute(LPVOID lpParam);

	//获取直播频道命令单独处理
	static UINT WINAPI Threead_Cmd_GetChannels(LPVOID lpVoid);

	static void _CurlHttpRequestFunc(CmdItem* pItem);

	static CmdManager*	m_pInstance;
	static CLiveLockEx	m_lockInst;

	HANDLE          m_hSemaphoreGetList = NULL;
};

#define AflGetCmdManager	CmdManager::GetInstance