#include "Vayo3dDevice.h"
#include "Vayo3dRoot.h"
#include "Vayo3dCamera.h"
#include "Vayo3dCoreGUI.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dSceneManager.h"

NS_VAYO3D_BEGIN

static TouchDispatcher* s_touchDispatcher = NULL;

float Device::getAspectRatio() const
{
	return static_cast<float>(_attribute.ScreenSize._width) / _attribute.ScreenSize._height;
}

Device::Device(int deviceID, const Attrib& attrib)
	: _deviceID(deviceID)
	, _attribute(attrib)
	, _appPaused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
	, _uiManager(NULL)
{
	s_touchDispatcher = TouchDispatcher::getSingletonPtr();
}

Device::~Device()
{
	SAFE_DELETE(_uiManager);
}

bool Device::openUI()
{
	_attribute.TurnOnUI = true;
	if (_uiManager)
		return true;

	_uiManager = new UIManager(this);
	if (!_uiManager || !_uiManager->init())
	{
		_attribute.TurnOnUI = false;
		SAFE_DELETE(_uiManager);
		return false;
	}

	return true;
}

void Device::closeUI()
{
	_attribute.TurnOnUI = false;
	SAFE_DELETE(_uiManager);
}

void Device::onResize()
{
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	if (pRenderer && pRenderer->isActive())
	{
		pRenderer->setViewpot(Recti(0, 0, _attribute.ScreenSize._width, _attribute.ScreenSize._height));
		Camera* pActiveCamera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
		if (pActiveCamera)
		{
			float w = _attribute.ScreenSize._width;
			float h = _attribute.ScreenSize._height;
			if (0 == _attribute.ScreenSize._height)
				h = 1.0f;

			if (pActiveCamera->isOrthogonal())
				pActiveCamera->setLens(w, h, pActiveCamera->getNearZ(), pActiveCamera->getFarZ());
			else
				pActiveCamera->setLens(pActiveCamera->getFovY(), w / h, pActiveCamera->getNearZ(), pActiveCamera->getFarZ());
		}
	}
}

void Device::setWndCaption(const wstring& wndCaption)
{
	_attribute.WndCaption = wndCaption;
}

void Device::setScreenSize(const Dimension2di& screenSize)
{
	_attribute.ScreenSize = screenSize;
}

void Device::setBgClearColor(unsigned int bgClr)
{
	_attribute.BgClearColor = bgClr;
}

wstring Device::getDeviceCode() const
{
	wstringstream ss;
	if (_deviceID == -1)
		ss << L"[MAIN_DEVICE]";
	else
		ss << L"[EXTRAS_DEVICE_" << _deviceID << L"]";
	return ss.str();
}

const wstring& Device::getWndCaption() const
{
	return _attribute.WndCaption;
}

const Dimension2di& Device::getScreenSize() const
{
	return _attribute.ScreenSize;
}

void Device::injectMouseDown(EMouseKeys mouseKey, int x, int y)
{
	Root::getSingleton().bootFrame(NULL, this);
	_mouseIsDown.addState(mouseKey);
	if (_mouseIsDown.checkState(EMK_LEFT))
		s_touchDispatcher->handleTouchBegan(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(EMK_RIGHT))
		s_touchDispatcher->handleTouchBegan(x, y, EMK_RIGHT);
	if (_mouseIsDown.checkState(EMK_MIDDLE))
		s_touchDispatcher->handleTouchBegan(x, y, EMK_MIDDLE);
}

void Device::injectMouseUp(EMouseKeys mouseKey, int x, int y)
{
	Root::getSingleton().bootFrame(NULL, this);
	BitState mouseIsUp = mouseKey;
	if (_mouseIsDown.checkState(EMK_LEFT) && mouseIsUp.checkState(EMK_LEFT))
	{
		_mouseIsDown.eraseState(EMK_LEFT);
		s_touchDispatcher->handleTouchEnded(x, y, EMK_LEFT);
	}
	if (_mouseIsDown.checkState(EMK_RIGHT) && mouseIsUp.checkState(EMK_RIGHT))
	{
		_mouseIsDown.eraseState(EMK_RIGHT);
		s_touchDispatcher->handleTouchEnded(x, y, EMK_RIGHT);
	}
	if (_mouseIsDown.checkState(EMK_MIDDLE) && mouseIsUp.checkState(EMK_MIDDLE))
	{
		_mouseIsDown.eraseState(EMK_MIDDLE);
		s_touchDispatcher->handleTouchEnded(x, y, EMK_MIDDLE);
	}
}

void Device::injectMouseMove(int x, int y)
{
	Root::getSingleton().bootFrame(NULL, this);
	s_touchDispatcher->setTouchCurPos(x, y);
	if (_mouseIsDown.checkState(EMK_LEFT))
		s_touchDispatcher->handleTouchMoved(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(EMK_RIGHT))
		s_touchDispatcher->handleTouchMoved(x, y, EMK_RIGHT);
	if (_mouseIsDown.checkState(EMK_MIDDLE))
		s_touchDispatcher->handleTouchMoved(x, y, EMK_MIDDLE);
}

void Device::injectMouseWheel(float wheel)
{
	Root::getSingleton().bootFrame(NULL, this);
	s_touchDispatcher->handleTouchWheel(wheel);
}

void Device::injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown)
{
	Root::getSingleton().bootFrame(NULL, this);
}

void Device::injectPaint()
{
	Root::getSingleton().bootFrame(NULL, this);
}

void Device::injectDestroy()
{
	Root::getSingleton().bootFrame(NULL, this);
}

void Device::injectActivate()
{
	Root::getSingleton().bootFrame(NULL, this);
	_appPaused = false;
	Root::getSingleton().getTimer().start();
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	if (pRenderer)
		pRenderer->restoreContext();
}

void Device::injectInactive()
{
	Root::getSingleton().bootFrame(NULL, this);
	_appPaused = true;
	Root::getSingleton().getTimer().stop();
}

void Device::injectSizeMinimized(int w, int h)
{
	Root::getSingleton().bootFrame(NULL, this);
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = true;
	_minimized = true;
	_maximized = false;
}

void Device::injectSizeMaximized(int w, int h)
{
	Root::getSingleton().bootFrame(NULL, this);
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = false;
	_minimized = false;
	_maximized = true;
	onResize();
}

void Device::injectSizeRestored(int w, int h)
{
	Root::getSingleton().bootFrame(NULL, this);
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	// Restoring from minimized state?
	if (_minimized)
	{
		_appPaused = false;
		_minimized = false;
		onResize();
	}
	// Restoring from maximized state?
	else if (_maximized)
	{
		_appPaused = false;
		_maximized = false;
		onResize();
	}
	else if (_resizing)
	{
		// If user is dragging the resize bars, we do not resize 
		// the buffers here because as the user continuously 
		// drags the resize bars, a stream of WM_SIZE messages are
		// sent to the window, and it would be pointless (and slow)
		// to resize for each WM_SIZE message received from dragging
		// the resize bars.  So instead, we reset after the user is 
		// done resizing the window and releases the resize bars, which 
		// sends a WM_EXITSIZEMOVE message.
	}
	else // API call such as SetWindowPos or mSwapChain->SetFullscreenState.
	{
		onResize();
	}
}

void Device::injectEnterSizeMove()
{
	Root::getSingleton().bootFrame(NULL, this);
	_appPaused = true;
	_resizing = true;
	Root::getSingleton().getTimer().stop();
}

void Device::injectExitSizeMove()
{
	Root::getSingleton().bootFrame(NULL, this);
	_appPaused = false;
	_resizing = false;
	Root::getSingleton().getTimer().start();
	onResize();
}

void Device::injectInputLanguageChange()
{
	Root::getSingleton().bootFrame(NULL, this);
}

NS_VAYO3D_END
