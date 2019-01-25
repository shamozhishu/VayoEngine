#include "Vayo2dCore.h"

NS_VAYO2D_BEGIN

static Core* s_d2d1Core = NULL;
Core::Core()
{
}

Core::~Core()
{
	this->discardDeviceResources();
	safeRelease(m_pD2DFactory);
	safeRelease(m_pWICFactory);
	safeRelease(m_pDWriteFactory);
	UnregisterClass(L"D2D1WndClass", nullptr);
}

bool Core::init(HINSTANCE hInstance, int nCmdShow)
{
	HRESULT hr = E_FAIL;

	WNDCLASSEX wcex = {sizeof(WNDCLASSEX)};
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = Core::wndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = sizeof(void*);
	wcex.hInstance = hInstance;
	wcex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	wcex.hbrBackground = nullptr;
	wcex.lpszMenuName = nullptr;
	wcex.lpszClassName = L"D2D1WndClass";
	wcex.hIcon = nullptr;

	// 注册窗口类
	RegisterClassExW(&wcex);

	// 计算窗口大小
	RECT window_rect = { 0, 0, 1080, 640 };
	DWORD window_style = WS_OVERLAPPEDWINDOW;
	AdjustWindowRect(&window_rect, window_style, FALSE);
	window_rect.right -= window_rect.left;
	window_rect.bottom -= window_rect.top;
	window_rect.left = (::GetSystemMetrics(SM_CXFULLSCREEN) - window_rect.right) / 2;
	window_rect.top = (::GetSystemMetrics(SM_CYFULLSCREEN) - window_rect.bottom) / 2;

	// 创建窗口
	m_hWnd = CreateWindowExA(0, "D2D1WndClass", m_wndCaption.c_str(), window_style, window_rect.left,
		window_rect.top, window_rect.right, window_rect.bottom, 0, 0, hInstance, this);

	// 显示窗口
	hr = m_hWnd ? S_OK : E_FAIL;
	if (SUCCEEDED(hr))
	{
		createDeviceIndependentResources();
		createDeviceResources();

		ShowWindow(m_hWnd, nCmdShow);
		UpdateWindow(m_hWnd);
	}

	return SUCCEEDED(hr);
}

void Core::updateScene(float dt)
{
}

void Core::drawScene()
{
}

void Core::calculateFrameStats()
{
	// 计算每秒平均帧数的代码，还计算了绘制一帧的平均时间
	// 这些统计信息会显示在窗口标题栏中
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// 计算一秒时间内的平均值
	float delta = m_timer.totalTime();
	delta -= timeElapsed;
	if (delta >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		std::ostringstream outs;
		outs.precision(6);
		outs << m_wndCaption.c_str() << " | "
			<< "FPS: " << fps << " | "
			<< "Frame Time: " << mspf << " (ms)";
		SetWindowTextA(m_hWnd, outs.str().c_str());

		// 为了计算下一个平均值重置一些值
		frameCnt = 0;
		timeElapsed += delta;
	}
}

bool Core::createDeviceIndependentResources()
{
	// 创建D2D工厂
	HRESULT hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		__uuidof(ID2D1Factory),
		reinterpret_cast<void**>(&m_pD2DFactory));

	// 创建WIC工厂
	if (SUCCEEDED(hr))
	{
		hr = CoCreateInstance(
			CLSID_WICImagingFactory,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_PPV_ARGS(&m_pWICFactory));
	}

	// 创建DirectWrite工厂
	if (SUCCEEDED(hr))
	{
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(m_pDWriteFactory),
			reinterpret_cast<IUnknown **>(&m_pDWriteFactory));
	}

	return SUCCEEDED(hr);
}

bool Core::createDeviceResources()
{
	HRESULT hr = S_OK;
	if (m_pHwndRT == NULL && SUCCEEDED(hr))
	{
		RECT rc;
		GetClientRect(m_hWnd, &rc);
		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		hr = m_pD2DFactory->CreateHwndRenderTarget(
			D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size, D2D1_PRESENT_OPTIONS_IMMEDIATELY),
			&m_pHwndRT);
	}

	return SUCCEEDED(hr);
}

void Core::discardDeviceResources()
{
	safeRelease(m_pHwndRT);
}

void Core::run()
{
	m_timer.reset();
	while (m_bRunning)
	{
		m_timer.tick();
		MSG msg;
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			if (createDeviceResources())
			{
				updateScene(m_timer.deltaTime());
				drawScene();
			}
			calculateFrameStats();
		}
	}
}

LRESULT Core::wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT result = 0;

	if (message == WM_CREATE)
	{
		LPCREATESTRUCT pcs = (LPCREATESTRUCT)lParam;
		Core* pD2DApp = (Core *)pcs->lpCreateParams;
		::SetWindowLongPtrW(hwnd, GWLP_USERDATA, PtrToUlong(pD2DApp));
		result = 1;
	}
	else
	{
		Core* pD2DApp = reinterpret_cast<Core*>(static_cast<LONG_PTR>(::GetWindowLongPtrW(hwnd, GWLP_USERDATA)));
		bool wasHandled = false;

		if (pD2DApp)
		{
			switch (message)
			{
			case WM_LBUTTONDOWN:
			case WM_MBUTTONDOWN:
			case WM_RBUTTONDOWN:
				pD2DApp->onMouseDown(wParam, LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_SIZE:
				pD2DApp->onResize(LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_LBUTTONUP:
			case WM_MBUTTONUP:
			case WM_RBUTTONUP:
				pD2DApp->onMouseUp(wParam, LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_MOUSEMOVE:
				pD2DApp->onMouseMove(wParam, LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_MOUSEWHEEL:
				pD2DApp->onMouseWheel(LOWORD(wParam), HIWORD(wParam), LOWORD(lParam), HIWORD(lParam));
				break;
			case WM_DESTROY:
				pD2DApp->onDestroy();
				result = 1;
				wasHandled = true;
				break;
			}
		}

		if (!wasHandled)
			result = DefWindowProc(hwnd, message, wParam, lParam);
	}

	return result;
}

void Core::onResize(UINT width, UINT height)
{
	if (m_pHwndRT != NULL)
	{
		D2D1_SIZE_U size;
		size.width = width;
		size.height = height;
		m_pHwndRT->Resize(size);
	}
}

void Core::onDestroy()
{
	m_bRunning = false;
}

NS_VAYO2D_END
