#include "stdafx.h"
#include "ModelView.h"
#include "ModelApp.h"

static ModelApp* g_app = NULL;
IMPLEMENT_DYNCREATE(CModelView, CView)
BEGIN_MESSAGE_MAP(CModelView, CView)
END_MESSAGE_MAP()
CModelView::CModelView()
	: m_bInitOK(false)
{
}

CModelView::~CModelView()
{
	g_app->cleanup();
	SAFE_DELETE(g_app);
}

bool CModelView::Init()
{
	if (!g_app)
	{
		Root::Config configInfo;
		configInfo.MainDeviceAttrib.BgClearColor = 0xff080808;
		configInfo.MainDeviceAttrib.WndHandle = GetSafeHwnd();
		configInfo.MainDeviceAttrib.WndQuit = false;
		g_app = new ModelApp(configInfo);
		m_bInitOK = Root::getSingleton().IsLaunched() && g_app->startup();
	}
	return m_bInitOK;
}

bool CModelView::IsOK() const
{
	return m_bInitOK;
}

void CModelView::OnDraw(CDC* pDC)
{
}

BOOL CModelView::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (m_bInitOK)
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
		case WM_GETMINMAXINFO:
			((MINMAXINFO*)lParam)->ptMinTrackSize.x = 200;
			((MINMAXINFO*)lParam)->ptMinTrackSize.y = 200;
			return 0;
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

void CModelView::PostNcDestroy()
{
	CView::PostNcDestroy();
}
