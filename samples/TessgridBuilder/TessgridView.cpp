#include "stdafx.h"
#include "TessgridView.h"
#include "TessgridApp.h"
#include "GridDataManager.h"

static TessgridApp* g_app = NULL;
IMPLEMENT_DYNCREATE(CTessgridView, CView)
BEGIN_MESSAGE_MAP(CTessgridView, CView)
END_MESSAGE_MAP()
CTessgridView::CTessgridView()
	: m_bIsStartup(false)
{
}

CTessgridView::~CTessgridView()
{
	CGridDataManager::Destroy();
	g_app->cleanup();
	SAFE_DELETE(g_app);
}

bool CTessgridView::Init()
{
	if (!g_app)
	{
		Root::Config configInfo;
		configInfo.MainDeviceAttrib.BgClearColor = 0xff080808;
		configInfo.MainDeviceAttrib.WndHandle = GetSafeHwnd();
		configInfo.MainDeviceAttrib.WndQuit = false;
		g_app = new TessgridApp(&configInfo);
		m_bIsStartup = Root::getSingleton().isLaunched() && g_app->startup();
	}
	return m_bIsStartup;
}

bool CTessgridView::IsStartup() const
{
	return m_bIsStartup;
}

void CTessgridView::OnDraw(CDC* pDC)
{
}

BOOL CTessgridView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (m_bIsStartup)
	{
		Device* dev = Root::getSingleton().getMainDevice();
		switch (message)
		{
		case WM_SIZE:
			if (wParam == SIZE_MINIMIZED)
				dev->injectSizeMinimized(LOWORD(lParam), HIWORD(lParam));
			else if (wParam == SIZE_MAXIMIZED)
				dev->injectSizeMaximized(LOWORD(lParam), HIWORD(lParam));
			else if (wParam == SIZE_RESTORED)
				dev->injectSizeRestored(LOWORD(lParam), HIWORD(lParam));
			return 0;
		case WM_MENUCHAR:
			return MAKELRESULT(0, MNC_CLOSE);
		case WM_LBUTTONDOWN:
			dev->injectMouseDown(EMK_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_RBUTTONDOWN:
			dev->injectMouseDown(EMK_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MBUTTONDOWN:
			dev->injectMouseDown(EMK_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_LBUTTONUP:
			dev->injectMouseUp(EMK_LEFT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_RBUTTONUP:
			dev->injectMouseUp(EMK_RIGHT, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MBUTTONUP:
			dev->injectMouseUp(EMK_MIDDLE, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_MOUSEMOVE:
			dev->injectMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			return 0;
		case WM_PAINT:
			dev->injectPaint();
			return 0;
		}
	}

	return CView::OnWndMsg(message, wParam, lParam, pResult);
}

void CTessgridView::PostNcDestroy()
{
	CView::PostNcDestroy();
}
