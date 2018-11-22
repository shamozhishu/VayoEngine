
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

// 实现
protected:
	HICON m_hIcon;
	CModelView* m_modelView;
	CMFCPropertyGridCtrl m_wndPropList;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
