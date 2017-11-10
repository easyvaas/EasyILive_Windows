#pragma once
#include <synchapi.h>
//#include "CommonExport.h"

class CLiveLockEx
{
public:

	CLiveLockEx()
	{
		::InitializeCriticalSection(&m_critSec);
	}
	virtual ~CLiveLockEx()
	{
		::DeleteCriticalSection(&m_critSec);
	}

	void Lock()
	{
		::EnterCriticalSection(&m_critSec);
	}
	void UnLock()
	{
		::LeaveCriticalSection(&m_critSec);
	}

private:
	CRITICAL_SECTION	m_critSec;
};