#pragma once
#include "GridData.h"
#include "BgCustomCtrl.h"

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

class CModelEditorCtrls
{
	friend class CMsgModelVisitor;
	friend class CTessgridBuilderDlg;
public:
	CModelEditorCtrls();
	void DisplayModel();
	bool Init(CTessgridBuilderDlg* dlg);
	void RefreshTreeCtrl();
	void ExpandTreeCtrl(HTREEITEM treeItem, bool expanding);
	void ShowPropGridCtrl(int gridCtrlShowState);
	void RefreshPropGridCtrl(ETreeCtrlItemType itemType, HTREEITEM hItem);

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

private:
	CString       m_savePath;
	CListCtrl     m_listCtrl;
	CTreeCtrl     m_treeCtrl;
	CImageList    m_listImg;
	CImageList    m_treeImg;
	CBgCustomCtrl m_staPropGridBg;

	HDITEM    m_gridCtrlItem;
	HTREEITEM m_hParentModelItem;
	HTREEITEM m_curSelTreeCtrlItem;
	CMFCPropertyGridCtrl m_propGridCtrl;

	int  m_curSelListCtrlItem;
	bool m_isTreeCtrlExpand;
	bool m_activeSelchangedAfxMsg;
	bool m_treeCtrlDeleteGridData;
	bool m_listCtrlDeleteGridData;
	ETreeCtrlItemType m_curSelItemType;
	CTessgridBuilderDlg* m_pMainDlg;
};
