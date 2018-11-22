
// ModelBuilderDlg.h : ͷ�ļ�
//

#pragma once

class CModelView;
// CModelBuilderDlg �Ի���
class CModelBuilderDlg : public CDialogEx
{
// ����
public:
	CModelBuilderDlg(CWnd* pParent = NULL);	// ��׼���캯��
	static CModelBuilderDlg* StartDlg(CWnd* pParent = NULL);
	static void FinishDlg(CModelBuilderDlg* &pDlg);
	bool isInitOK() const;
// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELBUILDER_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
	void InitPropGridCtrl();
	BOOL InitToolBar();
	void InitStatusBar();
	void Resize();

// ʵ��
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

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};
