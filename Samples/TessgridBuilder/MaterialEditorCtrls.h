#pragma once
#include "BgCustomCtrl.h"

class CMaterialEditorCtrls
{
	friend class CMsgMaterialVisitor;
	friend class CTessgridBuilderDlg;
public:
	CMaterialEditorCtrls();
	bool Init(CTessgridBuilderDlg* dlg);

private:
	CString              m_savePath;
	CListCtrl            m_listCtrl;
	CImageList           m_listImg;
	CBgCustomCtrl        m_staPropGridBg;
	HDITEM               m_gridCtrlItem;
	CMFCPropertyGridCtrl m_propGridCtrl;
};
