#pragma once

enum
{
	ETAB_MODEL,
	ETAB_MATERIAL,
	ETAB_SCENE
};

class CMsgVisitor
{
public:
	virtual ~CMsgVisitor() {}
	virtual void OnSize(CRect clientRc, CRect tabctrlRc) = 0;
	virtual void ShowCtrls(bool bShow) = 0;
	virtual void OnNewBuild() = 0;
	virtual void OnOpenFile() = 0;
	virtual void OnSaveFile() = 0;
	virtual void OnSaveAs() = 0;
	virtual void OnResetCamera();
	virtual void OnExpandTreeCtrl() = 0;
	virtual void OnChangeBgColor();
	virtual void OnContextMenu(CWnd* pWnd, CPoint point) = 0;
	virtual LRESULT OnPropertyChanged(WPARAM wParam, LPARAM lParam) = 0;
};
