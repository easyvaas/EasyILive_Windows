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

void CEVDemoCallBack::InitInstance_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::SetParameter_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetParameter_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::InitVideoWnd_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}
// 
// void CEVDemoCallBack::GetLID_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo, OUT char* strLid, OUT char* strKey)
// {
// 	if (!bSuccessed)
// 	{
// 		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
// 	}
// 	else
// 	{
// 		// 当成功时,返回推流地址;
// 		CString id = CA2CT(strLid);
// 		CString key = CA2CT(strKey);
// 		SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_GETLIDANDKEY_SUCCESS, WPARAM(id.GetString()), LPARAM(key.GetString()));
// 	}
// }

void CEVDemoCallBack::PrepareStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
	else
	{
		// 当成功时,返回推流地址;
		CString strPushUrl = CA2CT(strErrInfo);
		SendMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(), WM_PREPARESTREAM_CALLBACK, true, LPARAM(strPushUrl.GetString()));
	}
}

void CEVDemoCallBack::StartStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::StopStream_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::SetSelItemState_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::DeleteSelItem_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetGameOptions_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::AddScene_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::ModifyScene_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::SetMicropVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetMicropVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::SetDesktopVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetDesktopVolume_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetVideoDevices_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetAudioDevices_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetSelSceneClass_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}

void CEVDemoCallBack::GetSelSceneInfo_CallBack(IN const bool& bSuccessed, IN const char* strErrInfo)
{
	// 错误处理;
	if (!bSuccessed)
	{
		::MessageBoxA(NULL, strErrInfo, NULL, NULL);
	}
}