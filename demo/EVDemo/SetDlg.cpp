// SetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SetDlg.h"

// CSetDlg 对话框
IMPLEMENT_DYNAMIC(CSetDlg, CDialogEx)

CSetDlg::CSetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSetDlg::IDD, pParent)
{

}

CSetDlg::~CSetDlg()
{
}

void CSetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_RATE, m_cmbRate);
	DDX_Control(pDX, IDC_COMBO_FPS, m_cmbFPS);
	DDX_Control(pDX, IDC_COMBO_MAXBIT, m_cmbMaxBit);
	DDX_Control(pDX, IDC_EDIT_FPS, m_edtFps);
	DDX_Control(pDX, IDC_EDIT_MAXBIT, m_edtMaxbit);
}


BEGIN_MESSAGE_MAP(CSetDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSetDlg::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CSetDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	EVStreamerParameter para;
	AfxGetEVLive()->GetParameter(para);

	CString strRate;
	strRate.Format(_T("%d*%d"), para.videoResolutionWidth, para.videoResolutionHeight);
	m_cmbRate.AddString(_T("1920*1080"));
	m_cmbRate.AddString(_T("1280*720"));
	m_cmbRate.AddString(_T("960*540"));
	m_cmbRate.AddString(_T("640*360"));
	m_cmbRate.SetCurSel(1);
	for (int i = 0; i < m_cmbRate.GetCount(); i++)
	{
		CString strItem;
		m_cmbRate.GetLBText(i, strItem);
		if (0 == strItem.Compare(strRate))
		{
			m_cmbRate.SetCurSel(i);
			break;
		}
	}

	CString strFPS;
	strFPS.Format(_T("%d"), para.frameRate);
	/*m_cmbFPS.AddString(_T("15"));
	m_cmbFPS.AddString(_T("20"));
	m_cmbFPS.AddString(_T("25"));
	m_cmbFPS.AddString(_T("30"));
	m_cmbFPS.SetCurSel(2);
	for (int i = 0; i < m_cmbFPS.GetCount(); i++)
	{
		CString strItem;
		m_cmbFPS.GetLBText(i, strItem);
		if (0 == strItem.Compare(strFPS))
		{
			m_cmbFPS.SetCurSel(i);
			break;
		}
	}*/
	m_edtFps.SetWindowText(strFPS);

	CString strMaxBit;
	strMaxBit.Format(_T("%d"), para.maxVideoBitrate);
	/*m_cmbMaxBit.AddString(_T("500"));
	m_cmbMaxBit.AddString(_T("1000"));
	m_cmbMaxBit.AddString(_T("1500"));
	m_cmbMaxBit.AddString(_T("2000"));
	m_cmbMaxBit.AddString(_T("2500"));
	m_cmbMaxBit.AddString(_T("3000"));
	m_cmbMaxBit.SetCurSel(1);
	for (int i = 0; i < m_cmbMaxBit.GetCount(); i++)
	{
		CString strItem;
		m_cmbMaxBit.GetLBText(i, strItem);
		if (0 == strItem.Compare(strMaxBit))
		{
			m_cmbMaxBit.SetCurSel(i);
			break;
		}
	}*/
	m_edtMaxbit.SetWindowText(strMaxBit);

	return TRUE;
}

// CSetDlg 消息处理程序


void CSetDlg::OnBnClickedOk()
{
	CString strRate;
	CString strFPS;
	CString strMaxBit;
	m_cmbRate.GetWindowText(strRate);
	//m_cmbFPS.GetWindowText(strFPS);
	m_edtFps.GetWindowText(strFPS);
	//m_cmbMaxBit.GetWindowText(strMaxBit);
	m_edtMaxbit.GetWindowText(strMaxBit);

	EVStreamerParameter para;
	para.frameRate = _ttoi(strFPS);
	para.maxVideoBitrate = _ttoi(strMaxBit);
	int iPos = strRate.Find(_T("*"));
	para.videoResolutionWidth = _ttoi(strRate.Left(iPos));
	para.videoResolutionHeight = _ttoi(strRate.Right(strRate.GetLength() - (iPos + 1)));
	AfxGetEVLive()->SetParameter(para);

	CDialogEx::OnOK();
}
