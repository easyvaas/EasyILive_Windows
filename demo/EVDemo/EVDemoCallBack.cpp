#include "stdafx.h"
#include "EVDemoCallBack.h"

CEVDemoCallBack::CEVDemoCallBack()
{
}

CEVDemoCallBack::~CEVDemoCallBack()
{
}

// ���Խ�����ʵ�ַŵ��������cpp��ʵ��;

#if !TEST_PRO
void CEVDemoCallBack::InitSDK_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// ������;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}
#endif
