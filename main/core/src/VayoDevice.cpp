#include "VayoDevice.h"
#include "VayoCore.h"

NS_VAYO_BEGIN

static TouchDispatcher* s_touchDispatcher = NULL;

float Device::getAspectRatio() const
{
	return static_cast<float>(_attribute.ScreenSize._width) / _attribute.ScreenSize._height;
}

Device::Device(EDeviceID deviceID, const Attrib& attrib)
	: _deviceID(deviceID)
	, _attribute(attrib)
	, _appPaused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
{
	s_touchDispatcher = TouchDispatcher::getSingletonPtr();
}

Device::~Device()
{
}

void Device::onResize()
{
	Core::getCore().resize(this);
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
	if (_deviceID == EDID_MAIN_DEVICE)
		ss << L"EDID_MAIN_DEVICE";
	else
		ss << L"EDID_AUX_DEVICE" << _deviceID;
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
	Core::getCore().bootFrame(this);
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
	Core::getCore().bootFrame(this);
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
	Core::getCore().bootFrame(this);
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
	Core::getCore().bootFrame(this);
	s_touchDispatcher->handleTouchWheel(wheel);
}

void Device::injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown)
{
	Core::getCore().bootFrame(this);
}

void Device::injectPaint()
{
	Core::getCore().bootFrame(this);
}

void Device::injectDestroy()
{
	Core::getCore().bootFrame(this);
}

void Device::injectActivate()
{
	Core::getCore().bootFrame(this);
	_appPaused = false;
	Core::getCore().getTimer().start();
	Core::getCore().activate(this);
}

void Device::injectInactive()
{
	Core::getCore().bootFrame(this);
	_appPaused = true;
	Core::getCore().getTimer().stop();
}

void Device::injectSizeMinimized(int w, int h)
{
	Core::getCore().bootFrame(this);
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = true;
	_minimized = true;
	_maximized = false;
}

void Device::injectSizeMaximized(int w, int h)
{
	Core::getCore().bootFrame(this);
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = false;
	_minimized = false;
	_maximized = true;
	onResize();
}

void Device::injectSizeRestored(int w, int h)
{
	Core::getCore().bootFrame(this);
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
	Core::getCore().bootFrame(this);
	_appPaused = true;
	_resizing = true;
	Core::getCore().getTimer().stop();
}

void Device::injectExitSizeMove()
{
	Core::getCore().bootFrame(this);
	_appPaused = false;
	_resizing = false;
	Core::getCore().getTimer().start();
	onResize();
}

void Device::injectInputLanguageChange()
{
	Core::getCore().bootFrame(this);
}

NS_VAYO_END
