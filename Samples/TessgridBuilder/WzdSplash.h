#pragma once
#include "resource.h"

class CWzdSplash : public CWnd
{
	DECLARE_DYNAMIC(CWzdSplash)
public:
	CWzdSplash();
	~CWzdSplash();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CBitmap m_bitmap;
	void Create(UINT nBitmapID, CWnd* pParent);
	afx_msg void OnPaint();
};
