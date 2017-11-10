#pragma once
#include <vector>

// 截屏窗口;
class CWndSelDlg : public CDialog
{
	DECLARE_DYNAMIC(CWndSelDlg)

public:
	CWndSelDlg(CImage &image, CWnd* pParent = NULL);
	virtual ~CWndSelDlg();

	enum { IDD = IDD_SEL_WND };

	// 获取当前选择的区域;
	CRect GetSelRect();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);

	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);

	// 用于枚举所有窗口的窗口过程;
	static BOOL CALLBACK EnumProc(HWND hWnd, LPARAM lp);

	// 窗口信息节点;
	struct WndNode
	{
		HWND hWnd;
		CRect rc;

		// 排序专用;
		bool operator < (const WndNode &node)
		{
			return rc.Width() * rc.Height() < node.rc.Width() * node.rc.Height();
		}
	};
	typedef std::vector<WndNode> WndNodeVec;

	// 当前所有的可见窗口;
	WndNodeVec m_vecWnd;
	// 上层窗口输入的桌面截屏;
	CImage &m_Image;
	// 用户双缓存绘图的临时对象;
	CImage m_tmpImage;
	// 用户当前选中的区域;
	CRect m_rcSel;
	// 标识鼠标当前是否按下;
	bool m_bButtonDown;
	// 标识鼠标是否在按下状态被移动过;
	bool m_bDownMove;
	// 当前鼠标被按下的位置;
	CPoint m_ptButtonDown;
};
