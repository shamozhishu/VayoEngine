
// ModelBuilderDlg.h : 头文件
//

#pragma once

class CModelView;
// CModelBuilderDlg 对话框
class CModelBuilderDlg : public CDialogEx
{
// 构造
public:
	CModelBuilderDlg(CWnd* pParent = NULL);	// 标准构造函数
	static CModelBuilderDlg* StartDlg(CWnd* pParent = NULL);
	static void FinishDlg(CModelBuilderDlg* &pDlg);
	bool isInitOK() const;
// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELBUILDER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
	void InitPropGridCtrl();
	BOOL InitToolBar();
	void InitStatusBar();
	void Resize();

// 实现
protected:
	HICON m_hIcon;
	CModelView* m_modelView;
	CMenu m_mainMenu;
	CToolBar m_toolbar;
	CStatusBar m_statusbar;
	CStatic m_groupStatic;
	CImageList m_img;
	HDITEM m_gridCtrlItem;
	CMFCPropertyGridCtrl m_wndPropList;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};
