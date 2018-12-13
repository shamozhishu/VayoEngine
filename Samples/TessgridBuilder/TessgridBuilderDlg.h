
// TessgridBuilderDlg.h : ͷ�ļ�
//

#pragma once

class CTessgridView;
// CTessgridBuilderDlg �Ի���
class CTessgridBuilderDlg : public CDialog
{
// ����
public:
	CTessgridBuilderDlg(CWnd* pParent = NULL);	// ��׼���캯��
	static CTessgridBuilderDlg* StartDlg(CWnd* pParent = NULL);
	static void FinishDlg(CTessgridBuilderDlg* &pDlg);
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
	BOOL InitTreeCtrl();
	void Resize();
	void CheckToTree(HTREEITEM hItem, BOOL bCheck);
	void SetChildCheck(HTREEITEM hItem, BOOL bCheck);
	void SetParentCheck(HTREEITEM hItem, BOOL bCheck);

// ʵ��
protected:
	HICON m_hIcon;
	CTessgridView* m_modelView;
	CMenu m_mainMenu;
	CToolBar m_toolbar;
	CStatusBar m_statusbar;
	CImageList m_toolbarImg;
	HDITEM m_gridCtrlItem;
	CMFCPropertyGridCtrl m_wndPropList;
	CListCtrl m_listCtrl;
	HTREEITEM m_treeCircleItem;
	HTREEITEM m_treePolyItem;
	TVINSERTSTRUCT m_treeStruct;
	CImageList m_treeImg;
	CTreeCtrl m_treeCtrl;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnAboutMe();
	afx_msg void OnNewBuild();
	afx_msg void OnOpenFile();
	afx_msg void OnSaveFile();
	afx_msg void OnSaveAs();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInsertContour();
	afx_msg void OnDeleteContour();
	afx_msg void OnNMTVStateImageChangingTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
};
