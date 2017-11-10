#ifndef URLEX_DEFINE_5EC306DD_F9C8_4B1C_B42C_BE1493D26457_H_
#define URLEX_DEFINE_5EC306DD_F9C8_4B1C_B42C_BE1493D26457_H_

//http ����ʽ
const TCHAR HTTP_REQUEST_GET[]  = _T("GET");
const TCHAR HTTP_REQUEST_POST[] = _T("POST");
const TCHAR HTTP_REQUEST_PUT[]  = _T("PUT");

//uri�ӿ�method
const TCHAR URL_METHOD_LOGIN[]  = _T("/login/login");            
const TCHAR URL_METHOD_LOGOUT[] = _T("/login/logout");  //uri:"/login/logout"
const TCHAR URL_METHOD_LIVE_ADD[] = _T("/live/add");
//get
//ֱ���б�
const TCHAR URL_METHOD_GET_LIVECHANNEL[] = _T("/live/index"); //uri:"/live/index?page=1&pagenum=10"
//��ȡ������Ϣ
const TCHAR URL_METHOD_GET_ACCOUNT[] = _T("/account/index");  //uri:"/account/index
//ֱ������
const TCHAR URL_METHOD_GET_VIDEO_INFO[] = _T("/live/get_video_info"); //uri:"/live/get_video_info?vid=xxx"

// ���緵���ַ���;
#define E_OK                            "ok"
#define E_SESSION						"E_SESSION"
#define E_VIDEO_ALREADY_STOPPED			"E_VIDEO_ALREADY_STOPPED"
#define E_VIDEO_NOT_EXISTS				"E_VIDEO_NOT_EXISTS"
#define E_MAX_USER_MANAGER				"E_MAX_USER_MANAGER"
#define E_SERVER						"E_SERVER"


//���ݽṹ
typedef struct _tagURLEXRequestData
{
	CString strUrl; //�����ַ
	CString request_method;//POST GET
	CString url_method;    // /login/login��
	CString url_data;
}URLEXRequestData, *PURLEXRequestData;

typedef struct _tagUrlExLoginRetData
{
	CString login_ret_phone;
	CString login_ret_identity;
	CString login_ret_session_id;
	CString login_avatar;
}UrlExLoginRetData, *PUrlExLoginRetData;

#endif//URLEX_DEFINE_5EC306DD_F9C8_4B1C_B42C_BE1493D26457_H_
