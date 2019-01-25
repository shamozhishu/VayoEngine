#pragma once
#include "Vayo2dSupport.h"
#include "VayoTimer.h"

NS_VAYO2D_BEGIN

class _Vayo2dExport Core
{
public:
	Core();
	~Core();

	bool init(HINSTANCE hInstance, int nCmdShow);
	virtual void updateScene(float dt);
	virtual void drawScene();

	void run();
	static LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

protected:
	void calculateFrameStats();
	bool createDeviceIndependentResources();

	virtual bool createDeviceResources();
	virtual void discardDeviceResources();

	virtual void onMouseDown(WPARAM btnState, int x, int y) {}
	virtual void onMouseUp(WPARAM btnState, int x, int y) {}
	virtual void onMouseMove(WPARAM btnState, int x, int y) {}
	virtual void onMouseWheel(UINT nFlags, short zDelta, int x, int y) {}
	virtual void onResize(UINT width, UINT height);
	virtual void onDestroy();

protected:
	HINSTANCE                           m_hAppInst = nullptr;
	HWND                                m_hWnd = nullptr;
	std::string                         m_wndCaption = "D2D1App";
	bool                                m_bRunning = true;
	Timer                               m_timer;
	ID2D1Factory*                       m_pD2DFactory = nullptr;
	IWICImagingFactory*                 m_pWICFactory = nullptr;
	IDWriteFactory*                     m_pDWriteFactory = nullptr;
	ID2D1HwndRenderTarget*	            m_pHwndRT = nullptr;
};

NS_VAYO2D_END
