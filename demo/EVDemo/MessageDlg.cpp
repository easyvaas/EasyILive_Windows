#include "stdafx.h"
#include "resource.h"
#include "MessageDlg.h"
#include "EVDemoCallBack.h"

#define WM_MESSAGE_ARRIVE (WM_USER + 1)
#define WM_MESSAGE_CONNECTED (WM_USER + 2)
#define WM_MESSAGE_TOPICJOINED (WM_USER + 3)
#define WM_MESSAGE_ERROR (WM_USER + 4)
#define WM_MESSAGE_SERVERREADY (WM_USER + 5)
#define WM_MESSAGE_USERENTER (WM_USER + 6)
#define WM_MESSAGE_USERLEAVE (WM_USER + 7)
#define WM_MESSAGE_TOPICLEAVED (WM_USER + 8)

HWND g_hWnd = NULL;

void CEVDemoCallBack::onGetMessageServer(const char * serverIp, int serverPort, const char * handShakeKey)
{
	// g_message->SetParameter(serverIp, serverPort, "yizhibo", "", "pc_addDemo", handShakeKey);
	::SendMessage(g_hWnd, WM_MESSAGE_SERVERREADY, NULL, NULL);
}

void CEVDemoCallBack::onConnectError(const char * errorInfo)
{
}

void CEVDemoCallBack::onConnected()
{
	::SendMessage(g_hWnd, WM_MESSAGE_CONNECTED, NULL, NULL);
}

void CEVDemoCallBack::onJoinOK(const char * topic, const char ** users, int userNum,
	const char ** privateInfo, int privateInfoNum,
	const char ** managers, int managersNum,
	const char ** shutups, int shutupsNum,
	const char ** kickusers, int kickusersNum)
{
	::SendMessage(g_hWnd, WM_MESSAGE_TOPICJOINED, NULL, NULL);
}

void CEVDemoCallBack::onLeaveOK(const char * content, const char * topic)
{
	::SendMessage(g_hWnd, WM_MESSAGE_TOPICLEAVED, NULL, NULL);
}

void CEVDemoCallBack::onNewMessage(const char * message, const char * topic, const char * userData, const char * userId)
{
	OutputDebugStringA(message);
	OutputDebugStringA("\r\n");
	std::string strMsg = userId;
	strMsg.append("\r\n");
	strMsg.append(message);
	::SendMessage(g_hWnd, WM_MESSAGE_ARRIVE, (WPARAM)strMsg.c_str(), (LPARAM)userData);
}

void CEVDemoCallBack::onUserJoin(char ** users, int num)
{
	::SendMessage(g_hWnd, WM_MESSAGE_USERENTER, (WPARAM)users, (LPARAM)num);
}

void CEVDemoCallBack::onUserLeave(char ** users, int num)
{
	::SendMessage(g_hWnd, WM_MESSAGE_USERLEAVE, (WPARAM)users, (LPARAM)num);
}

void CEVDemoCallBack::onLikeCount(int likeCount)
{
	return;
}

void CEVDemoCallBack::onCommentCount(int likeCount)
{
	return;
}

void CEVDemoCallBack::onRiceRoll(int riceCount)
{
	return;
}

void CEVDemoCallBack::onWatchingCount(int watchingCount)
{
	return;
}

void CEVDemoCallBack::onWatchedCount(int watchedCount)
{
	return;
}


void CEVDemoCallBack::onError(int errCode)
{
	::SendMessage(g_hWnd, WM_MESSAGE_ERROR, (WPARAM)errCode, (LPARAM)NULL);
}


IMPLEMENT_DYNAMIC(CMessageDlg, CDialogEx)

CMessageDlg::CMessageDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMessageDlg::IDD, pParent)
{

}

CMessageDlg::~CMessageDlg()
{
}

void CMessageDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EDIT_MESSAGE, m_msgShowEdit);
    DDX_Control(pDX, IDC_EDIT_INPUT, m_editInput);
    DDX_Control(pDX, IDC_EDIT_TOPICNAME, m_editTopicName);
    DDX_Control(pDX, IDC_EDIT_SENDERNAME, m_editSenderName);
    DDX_Control(pDX, IDC_EDIT_USERID, m_editUserId);
}


BEGIN_MESSAGE_MAP(CMessageDlg, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_SEND, &CMessageDlg::OnBnClickedButtonSend)
    ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CMessageDlg::OnBnClickedButtonClear)
    ON_BN_CLICKED(IDOK, &CMessageDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CMessageDlg::OnBnClickedCancel)
    ON_WM_CLOSE()
    ON_BN_CLICKED(IDC_BUTTON_JOINTOPIC, &CMessageDlg::OnBnClickedButtonJointopic)
    ON_BN_CLICKED(IDC_BUTTON_LEAVETOPIC, &CMessageDlg::OnBnClickedButtonLeavetopic)
    ON_BN_CLICKED(IDC_BUTTON_DISCONNECT, &CMessageDlg::OnBnClickedButtonDisconnect)
    ON_BN_CLICKED(IDC_BUTTON_CONNECT, &CMessageDlg::OnBnClickedButtonConnect)
    ON_BN_CLICKED(IDC_BUTTON_LIKE, &CMessageDlg::OnBnClickedButtonLike)
END_MESSAGE_MAP()

BOOL CMessageDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  在此添加额外的初始化
    g_hWnd = m_hWnd;
    char serverIp[64] = {0};
    int serverPort = 0;
    char  strHandeSaheKey[512] = {0};

    m_editTopicName.SetWindowText(_T("system"));
    // m_editSenderName.SetWindowText(_T("pc_user"));
    m_editUserId.SetWindowText(APP_USERNAME.c_str());

	g_hWnd = m_hWnd;
	//if (g_message == NULL)
	//{
 //       // "yizhibo", "", "pc_addDemo"
 //       g_message = AfxGetEVLive()->Create_CEVMessage(g_messageCallback, "", "", "");
	//}

    // AfxGetEVLive()->GetMessageServer(serverIp, &serverPort, strHandeSaheKey);

    // AfxGetEVLive()->GetMessageServer
    // CEVMessage::GetMessageServer(serverIp, &serverPort, strHandeSaheKey);
    // "123.57.240.208"
	//  3081;

    ////     g_message = new CEVMessage(g_messageCallback);
    //g_message->SetParameter(serverIp, serverPort, "yizhibo", "", "pc_addDemo", strHandeSaheKey);
    //g_message->ConnectMsgSrv();
    //// AflGetOBSManager()->GetEVMessage()->CreateTopic("message", true, false, false);
    //char * topics[1] = { T2A(m_strTopicName) };
    //g_message->JoinTopic(topics, 1);
    // AflGetOBSManager()->GetEVMessage()->LeaveTopic(topics, 1);
    return TRUE;  // return TRUE unless you set the focus to a control
    // 异常:  OCX 属性页应返回 FALSE
}

wchar_t* TransformUTF8ToUnicodeM(const char* _str)
{
    int textlen = 0;
    wchar_t * result = NULL;
    if (_str)
    {
        textlen = MultiByteToWideChar(CP_UTF8, 0, _str, -1, NULL, 0);
        result = (wchar_t *)new wchar_t[(textlen + 1)];
        memset(result, 0, (textlen + 1)*sizeof(wchar_t));
        MultiByteToWideChar(CP_UTF8, 0, _str, -1, (LPWSTR)result, textlen);
    }
    return result;
}

char* TransformUnicodeToUTF8M(const wchar_t* _str)
{
    char* result = NULL;
    int textlen = 0;
    if (_str)
    {
        textlen = WideCharToMultiByte(CP_UTF8, 0, _str, -1, NULL, 0, NULL, NULL);
        result = (char *)new char[(textlen + 1)];
        memset(result, 0, sizeof(char)* (textlen + 1));
        WideCharToMultiByte(CP_UTF8, 0, _str, -1, result, textlen, NULL, NULL);
    }
    return result;
}

void CMessageDlg::OnBnClickedButtonSend()
{
    // TODO:  在此添加控件通知处理程序代码;
USES_CONVERSION;
	
    CString strContent;
    m_editInput.GetWindowText(strContent);
    char * utf8code = TransformUnicodeToUTF8M(strContent.GetBuffer());
    strContent.ReleaseBuffer();

    CString strFilterMsg;
    m_editSenderName.GetWindowText(strFilterMsg);
    AfxGetEVLive()->Send(T2A(m_strTopicName), T2A(strFilterMsg), utf8code);
    delete[] utf8code;
    utf8code = NULL;
}

void CMessageDlg::OnBnClickedButtonClear()
{
    // TODO:  在此添加控件通知处理程序代码
    m_msgShowEdit.SetWindowText(_T(""));
}


BOOL CMessageDlg::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    // TODO:  在此添加专用代码和/或调用基类
    USES_CONVERSION;
    switch (message)
    {
    case WM_MESSAGE_ARRIVE:
    {
                        CString strContent;
                        m_msgShowEdit.GetWindowText(strContent);
                        strContent.Append(_T("from:"));
                        wchar_t * punicode = TransformUTF8ToUnicodeM((char *)wParam);
                        strContent.Append(punicode);
                        delete[] punicode;
                        punicode = NULL;
                        strContent.Append(_T("\r\n"));

                        punicode = TransformUTF8ToUnicodeM((char *)lParam);
                        strContent.Append(punicode);
                        delete[] punicode;
                        punicode = NULL;
                        strContent.Append(_T("\r\n"));
                        m_msgShowEdit.SetWindowText(strContent);
    }
        break;

    case WM_MESSAGE_CONNECTED:
    {
                                 CString strContent;
                                 m_msgShowEdit.GetWindowText(strContent);
                                 strContent.Append(_T("connected\r\n"));
                                 m_msgShowEdit.SetWindowText(strContent);
                                 char * topics[1] = { T2A(m_strTopicName) };
                                 // g_message->JoinTopic(topics, 1);
    }
        break;

    case WM_MESSAGE_TOPICJOINED:
    {
                                   CString strContent;
                                   m_msgShowEdit.GetWindowText(strContent);
                                   strContent.Append(_T("joined\r\n"));
                                   m_msgShowEdit.SetWindowText(strContent);
    }
        break;
    case WM_MESSAGE_TOPICLEAVED:
    {
                                   CString strContent;
                                   m_msgShowEdit.GetWindowText(strContent);
                                   strContent.Append(_T("leaved\r\n"));
                                   m_msgShowEdit.SetWindowText(strContent);
    }
        break;
    // case WM_MESSAGE_SERVERREADY:
    {
                                   
                                   // g_message->ConnectMsgSrv();
                                   // AflGetOBSManager()->GetEVMessage()->CreateTopic("message", true, false, false);
                                   
                                   // AflGetOBSManager()->GetEVMessage()->LeaveTopic(topics, 1);
    }
        // break;
    case WM_MESSAGE_USERENTER:
    {
                                 CString strContent;
                                 m_msgShowEdit.GetWindowText(strContent);
                                 char ** p = (char **)wParam;
                                 for (int i = 0; i < (int)lParam; ++i)
                                 {
                                     wchar_t * punicode = TransformUTF8ToUnicodeM((char *)p[i]);
                                     strContent.Append(punicode);
                                     delete[] punicode;
                                     punicode = NULL;
                                     strContent.Append(_T(" enter!!!\r\n"));
                                 }
                                 
                                 m_msgShowEdit.SetWindowText(strContent);
    }
        break;
    case WM_MESSAGE_USERLEAVE:
    {
                                 CString strContent;
                                 m_msgShowEdit.GetWindowText(strContent);
                                 char ** p = (char **)wParam;
                                 for (int i = 0; i < (int)lParam; ++i)
                                 {
                                     wchar_t * punicode = TransformUTF8ToUnicodeM((char *)p[i]);
                                     strContent.Append(punicode);
                                     delete[] punicode;
                                     punicode = NULL;
                                     strContent.Append(_T(" leave!!!\r\n"));
                                 }

                                 m_msgShowEdit.SetWindowText(strContent);
    }
        break;
    case WM_MESSAGE_ERROR:
    {
                             CString strContent;
                             m_msgShowEdit.GetWindowText(strContent);
                             CString strError;
                             strError.Format(_T("error %d\r\n"), (int)wParam);
                             strContent.Append(strError);
                             m_msgShowEdit.SetWindowText(strContent);
    }
        break;

    default:
        break;
    }
    return CDialogEx::OnWndMsg(message, wParam, lParam, pResult);
}


void CMessageDlg::OnBnClickedOk()
{
    // TODO:  在此添加控件通知处理程序代码
    ShowWindow(SW_HIDE);
    // CDialogEx::OnOK();
}


void CMessageDlg::OnBnClickedCancel()
{
    // TODO:  在此添加控件通知处理程序代码
    ShowWindow(SW_HIDE);
    // CDialogEx::OnCancel();
}


void CMessageDlg::OnClose()
{
    // TODO:  在此添加消息处理程序代码和/或调用默认值
    ShowWindow(SW_HIDE);
    // CDialogEx::OnClose();
}


void CMessageDlg::OnBnClickedButtonJointopic()
{
    // TODO:  在此添加控件通知处理程序代码
    USES_CONVERSION;
    m_editTopicName.GetWindowText(m_strTopicName);
    char * topics[1] = { T2A(m_strTopicName) };
    AfxGetEVLive()->JoinTopic(topics, 1);
}


void CMessageDlg::OnBnClickedButtonLeavetopic()
{
    // TODO:  在此添加控件通知处理程序代码
    USES_CONVERSION;
    char * topics[1] = { T2A(m_strTopicName)};
	AfxGetEVLive()->LeaveTopic(topics, 1);
}


void CMessageDlg::OnBnClickedButtonDisconnect()
{
    // TODO:  在此添加控件通知处理程序代码
	AfxGetEVLive()->Close();
    CString strContent;
    m_msgShowEdit.GetWindowText(strContent);
    strContent.Append(_T("disconnected\r\n"));
    m_msgShowEdit.SetWindowText(strContent);
}


void CMessageDlg::OnBnClickedButtonConnect()
{
    // TODO:  在此添加控件通知处理程序代码

	AfxGetEVLive()->Connect();
}


void CMessageDlg::OnBnClickedButtonLike()
{
    // TODO:  在此添加控件通知处理程序代码
    USES_CONVERSION;
    
	AfxGetEVLive()->AddLikeCount(T2A(m_strTopicName), 1);
}
