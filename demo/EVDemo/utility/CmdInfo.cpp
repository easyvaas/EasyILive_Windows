#include "stdafx.h"
#include "CmdInfo.h"
#include <tchar.h>
#include "utility/JsonValueEx.h"

CmdManager* CmdManager::m_pInstance = NULL;
CLiveLockEx CmdManager::m_lockInst;

CmdManager::CmdManager(): m_bStop(true), m_hThCmd(NULL)
{
}

CmdManager::~CmdManager()
{
}

void CmdManager::AddCmd(IN TString& strCmd, IN Curl_CallBack pFunc /*= NULL*/)
{
	CmdItem cmdItem;
	cmdItem.cmdInfo.strCmd = strCmd;
	cmdItem.pFunc = pFunc;
	AddCmd(cmdItem);
}

void CmdManager::AddCmd(IN CmdItem& cmdInfo)
{
	if (NULL == m_hThCmd)
	{
		m_hThCmd = CreateThread(NULL, 0, Th_Cmd, this, 0, &m_dwThreadID); // �����߳�;
		m_bStop = false;
	}
	m_lockList.Lock();
	m_listCmd.push_back(cmdInfo);

	m_lockList.UnLock();
}

/*
add by sbd 2017.6.3
���������ȡƵ���б���Ϣ����
��ȡ�б�����ӿ�ͨ����ҳ��ʽʵ�֣�
��������������Ŷӣ�ÿ������˳����
����Ƶ���б����������
�ӹ��������׷�������->��������У�m_lockListGetChannels��->�������
->Threead_Cmd_GetChannels�߳�ѭ��ȡ������ -> ����ɹ�������ɣ�_CurlHttpRequestFunc��
->�������ݲ�����->���ط���ͼ->�����������������ݳɹ�
ֻ�����������ˣ��Ż�ִ����һ��Ƶ�����������ʵ���Ѿ��������ֻ���ڵȴ���
ע��WaitForSingleObject��λ�ã���AddCmdGetChannels������������ʱδ�����ƣ�
�����ڴ�m_lockListGetChannelsȡ������ʱ��Ҫ����һ����������Ƿ���ɣ�δ���˯�ߵȴ���

������������˳��ִ�У����Զ�CLiveChannelManage_Imp���ڲ����ݵ��޸ģ����ǰ����Ⱥ�˳��ִ�е�
*/

void CmdManager::AddCmdGetChannels(IN CmdItem& cmdItem, HANDLE& hSemaphore)
{
	if (m_hSemaphoreGetList == NULL &&	hSemaphore != NULL)
		m_hSemaphoreGetList = hSemaphore;

	if (NULL == m_hThreadCmdGetChannels)
	{
		m_hThreadCmdGetChannels = (HANDLE)
			_beginthreadex(NULL, 0, Threead_Cmd_GetChannels, (LPVOID)this, 0, &m_dwThreadIDGetChannels);
	}
	m_lockListGetChannels.Lock();
	m_listCmdGetChannels.push_back(cmdItem);
	m_lockListGetChannels.UnLock();
}

void CmdManager::DeInit()
{
	m_bStop = true;
	if (WAIT_TIMEOUT == WaitForSingleObject(m_hThCmd, 20))
	{
		TerminateThread(m_hThCmd, 0x10);
	}
	m_hThCmd = NULL;
}

DWORD WINAPI CmdManager::Th_Cmd(LPVOID lpParam)
{
	CmdManager *pThis = static_cast<CmdManager*>(lpParam);
	ASSERT(pThis);

    CmdList list;
	while ( (!pThis->m_bStop) && (pThis->m_hThCmd) )
	{
		list.clear();
		pThis->m_lockList.Lock();
		pThis->m_listCmd.swap(list);
		pThis->m_lockList.UnLock();
		if (list.empty())
		{
			Sleep(1);
			continue;
		}

		CmdList::iterator it = list.begin();
		for (; it != list.end(); ++it)
		{
			it->cmdInfo.bSucess = true;
			it->cmdInfo.jsResult.clear();
			it->cmdInfo.iErrorCode = 0;
			it->cmdInfo.strError = _T("");

			//it->cmdInfo.url_ex_data 
			CmdItem* pItem = new CmdItem(*it);
			DWORD dwThreadID = 0;
			CreateThread(NULL, 0, Th_Cmd_Excute, (LPVOID)pItem, 0, &dwThreadID); // �����߳�;
		}
	}
	return 0;
}

DWORD CmdManager::Th_Cmd_Excute(LPVOID lpParam)
{
	CmdItem* pItem = static_cast<CmdItem*>(lpParam);
	assert(pItem);
	
	std::string strResult = "";
	
	_CurlHttpRequestFunc(pItem);

	delete pItem;
	pItem = NULL;

	return 1;
}

UINT WINAPI CmdManager::Threead_Cmd_GetChannels(LPVOID lpVoid)
{
	CmdManager *pThis = static_cast<CmdManager*>(lpVoid);
	ASSERT(pThis);

	CmdList list;
	while ((!pThis->m_bStopGetChannels) && (pThis->m_hThreadCmdGetChannels))
	{
		
		list.clear();
		pThis->m_lockListGetChannels.Lock();
		pThis->m_listCmdGetChannels.swap(list);
		pThis->m_lockListGetChannels.UnLock();
		if (list.empty())
		{
			Sleep(1);
			continue;
		}

		CmdList::iterator it = list.begin();
		for (; it != list.end(); ++it)
		{
			WaitForSingleObject(pThis->m_hSemaphoreGetList, 0);
			it->cmdInfo.bSucess = true;
			it->cmdInfo.jsResult.clear();
			it->cmdInfo.iErrorCode = 0;
			it->cmdInfo.strError = _T("");

			//it->cmdInfo.url_ex_data 
			CmdItem* pItem = new CmdItem(*it);
			DWORD dwThreadID = 0;
		//	WaitForSingleObject(pItem->cmdInfo.m_hSemaphoreGetList, 0);
		    
			_CurlHttpRequestFunc(pItem);

			delete pItem;
			pItem = NULL;
		}
	}
	return 1;
}

void CmdManager::_CurlHttpRequestFunc(CmdItem* pItem)
{
	CCurlEx curlEx;

	if (pItem->cmdInfo.url_ex_data.request_method == HTTP_REQUEST_POST)
	{
		if (!curlEx.HTTP_RequestData_Post(
			pItem->cmdInfo.url_ex_data.strUrl,
			pItem->cmdInfo.url_ex_data.url_method,
			pItem->cmdInfo.url_ex_data.url_data,
			pItem->cmdInfo.jsResult
			))
		{
			pItem->cmdInfo.bSucess = false;
			pItem->cmdInfo.strError = curlEx.GetErrorInfo();
		}
	}
	else if (pItem->cmdInfo.url_ex_data.request_method == HTTP_REQUEST_GET)
	{
		if (!curlEx.HTTP_RequestData_Get(pItem->cmdInfo.jsResult, pItem->cmdInfo.strCmd.c_str(), pItem->cmdInfo.bVerifyRes))
		{
			pItem->cmdInfo.bSucess = false;
			//it->cmdInfo.iErrorCode = _ttoi(it->cmdInfo.jsResult["state"].GetTString().c_str());
			pItem->cmdInfo.strError = curlEx.GetErrorInfo();
		}
	}

	if (NULL != pItem->pFunc)
	{
		pItem->pFunc(pItem->cmdInfo);
	}
}

CmdManager* CmdManager::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_lockInst.Lock();
		if (NULL == m_pInstance)
		{
			m_pInstance = new CmdManager();
		}
		m_lockInst.UnLock();
	}
	return m_pInstance;
}

void CmdManager::ReleaseInstance()
{
	if (NULL != m_pInstance)
	{
		m_lockInst.Lock();
		if (NULL != m_pInstance)
		{
			m_pInstance->DeInit();
			delete m_pInstance;
			m_pInstance = NULL;
		}
		m_lockInst.UnLock();
	}
}