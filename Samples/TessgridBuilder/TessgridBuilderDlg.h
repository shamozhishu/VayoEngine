// TessgridBuilderDlg.h : 头文件
//

#pragma once
#include "GridData.h"

class CTessgridView;
// CTessgridBuilderDlg 对话框
class CTessgridBuilderDlg : public CDialog
{
// 构造
public:
	CTessgridBuilderDlg(CWnd* pParent = NULL);	// 标准构造函数
	static CTessgridBuilderDlg* StartDlg(CWnd* pParent = NULL);
	static void FinishDlg(CTessgridBuilderDlg* &pDlg);
	bool IsEngineInit() const;
	void DisplayModel();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MODELBUILDER_DIALOG };
#endif

protected:
	enum
	{
		EPGC_HIDE_ALL = ESTATE_00,
		EPGC_PROP = ESTATE_01,
		EPGC_PLACE = ESTATE_02,
		EPGC_CIRCLE = ESTATE_03,
		EPGC_POLY = ESTATE_04,
		EPGC_STRETCH = ESTATE_05,
		EPGC_TOPCAP = ESTATE_06,
		EPGC_BOTTOMCAP = ESTATE_07
	};
	enum ETreeCtrlItemType
	{
		EICON_NONE = -1,
		EICON_CIRCLE = 0,
		EICON_POLYGON,
		EICON_MODEL,
		EICON_CIRCLECONTOUR,
		EICON_POLYCONTOUR,
		EICON_POLYPOINT
	};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnCancel();
	void InitPropGridCtrl();
	void ShowPropGridCtrl(int gridCtrlShowState);
	void RefreshPropGridCtrl(ETreeCtrlItemType itemType, HTREEITEM hItem);
	BOOL InitToolBar();
	void InitStatusBar();
	BOOL InitListCtrl();
	BOOL InitTreeCtrl();
	void RefreshTreeCtrl();
	void ExpandTreeCtrl(HTREEITEM treeItem, bool expanding);
	void Resize();

private:
	void NeedRefreshModel();
	void ResetCtrlLayout();
	void RecursionShowPropGridCtrl(BOOL bShow, CMFCPropertyGridProperty* pGridProperty);
	bool SetPropAndPlaceGridCtrlValue(const CGridData* pGridData);
	bool SetTopAndBottomCapGridCtrlValue(const CGridData* pGridData);
	void SetTopAndBottomCapGridCtrlValue(const CGridShapeOp& shapeOp);
	void SetStretchBodyGridCtrlValue(const CGridShapeOp::SStretchingBody& shapeOpStretchBody, int iItem);
	void TopCapPropertyChanged(CString propName, CString propValue);
	void BottomCapPropertyChanged(CString propName, CString propValue);

// 实现
protected:
	HICON m_hIcon;
	CString m_savePath;
	CTessgridView* m_modelView;
	CMenu m_mainMenu;
	CToolBar m_toolbar;
	CImageList m_toolbarImg;
	CStatusBar m_statusbar;
	HDITEM m_gridCtrlItem;
	CMFCPropertyGridCtrl m_wndPropList;
	bool m_activeSelchangedAfxMsg;
	bool m_treeCtrlDeleteGridData;
	bool m_listCtrlDeleteGridData;
	CListCtrl m_listCtrl;
	CImageList m_listCtrlImg;
	bool m_isTreeCtrlExpand;
	CTreeCtrl m_treeCtrl;
	CImageList m_treeImg;
	ETreeCtrlItemType m_curSelItemType;
	HTREEITEM m_hParentModelItem;
	HTREEITEM m_curSelTreeCtrlItem;
	int m_curSelListCtrlItem;
	CStatic m_staStretchNum;

	// 生成的消息映射函数
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
	afx_msg void OnResetCamera();
	afx_msg void OnExpandTreeCtrl();
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnInsertContour();
	afx_msg void OnDeleteContour();
	afx_msg void OnAddStretchBody();
	afx_msg void OnRemoveStretchBody();
	afx_msg void OnTvnSelchangedTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTvnDeleteitemTreeContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnDeleteitemListContourIdx(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam);
};
