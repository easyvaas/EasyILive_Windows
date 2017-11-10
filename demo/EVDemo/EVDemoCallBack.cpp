#include "stdafx.h"
#include "EVDemoCallBack.h"

CEVDemoCallBack::CEVDemoCallBack()
{
}

CEVDemoCallBack::~CEVDemoCallBack()
{
}

// 可以将具体实现放到具体界面cpp内实现;

#if !TEST_PRO
void CEVDemoCallBack::InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}
#endif
