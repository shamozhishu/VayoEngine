#pragma once

#include "ModelEditorCtrls.h"
#include "MaterialEditorCtrls.h"

class CTessgridView;
class CTessgridBuilderDlg : public CDialog
{
	friend class CMsgVisitor;
	friend class CMsgModelVisitor;
	friend class CMsgMaterialVisitor;
	friend class CModelEditorCtrls;
	friend class CMaterialEditorCtrls;
public:
	CTessgridBuilderDlg(CWnd* pParent = NULL);
	~CTessgridBuilderDlg();
	static CTessgridBuilderDlg* StartDlg(CWnd* pParent = NULL);
	static void FinishDlg(CTessgridBuilderDlg* &pDlg);
	LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
	bool IsEngineInit() const;
	void DisplayModel();

#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELBUILDER_DIALOG };
#endif

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
	BOOL InitToolBar();
	void InitStatusBar();
	void ResizeWindow();
	DECLARE_MESSAGE_MAP()

protected:
	HICON          m_hIcon;
	CMenu          m_mainMenu;
	CToolBar       m_toolbar;
	CImageList     m_toolbarImg;
	CStatusBar     m_statusbar;
	CMFCTabCtrl    m_tabCtrl;
	CTessgridView* m_modelView;
	CMsgVisitor*   m_pMsgVisitor;
	CModelEditorCtrls m_modelEditorCtrls;
	CMaterialEditorCtrls m_materialEditorCtrls;

protected:
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnToolTipText(UINT nID, NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnAboutMe();
	afx_msg void OnNewBuild();
	afx_msg void OnOpenFile();
	afx_msg void OnSaveFile();
	afx_msg void OnSaveAs();
	afx_msg void OnResetCamera();
	afx_msg void OnExpandTreeCtrl();
	afx_msg void OnChangeBgColor();
	afx_msg void OnInsertContour();
	afx_msg void OnDeleteContour();
	afx_msg void OnAddStretchBody();
	afx_msg void OnRemoveStretchBody();
	afx_msg LRESULT OnActiveTabChanging(WPARAM wParam, LPARAM lParam);
	afx_msg void OnTvnSelchangedTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitemTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
};
