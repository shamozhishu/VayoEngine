#pragma once

class CModelView : public CView
{
public:
	CModelView();
	~CModelView();
	bool Init();
	bool IsOK() const;

protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	void PostNcDestroy();
	DECLARE_MESSAGE_MAP()
	DECLARE_DYNCREATE(CModelView)

private:
	bool m_bInitOK;
};
