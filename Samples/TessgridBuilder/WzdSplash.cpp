#include "stdafx.h"
#include "WzdSplash.h"

IMPLEMENT_DYNAMIC(CWzdSplash, CWnd)
CWzdSplash::CWzdSplash()
{
}

CWzdSplash::~CWzdSplash()
{
}

BEGIN_MESSAGE_MAP(CWzdSplash, CWnd)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CWzdSplash::Create(UINT nBitmapID, CWnd* pParent)
{
	m_bitmap.LoadBitmap(nBitmapID);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	CreateEx(0, AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		NULL, WS_POPUP | WS_VISIBLE, 0, 0, bitmap.bmWidth, bitmap.bmHeight, pParent->m_hWnd, NULL);
}

void CWzdSplash::OnPaint()
{
	CPaintDC dc(this);
	BITMAP bitmap;
	m_bitmap.GetBitmap(&bitmap);
	CDC dcComp;
	dcComp.CreateCompatibleDC(&dc);
	CBitmap* pOld = dcComp.SelectObject(&m_bitmap);
	dc.BitBlt(0, 0, bitmap.bmWidth, bitmap.bmHeight, &dcComp, 0, 0, SRCCOPY);
	dcComp.SelectObject(pOld);
}
