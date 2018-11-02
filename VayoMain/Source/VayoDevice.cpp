#include "VayoDevice.h"
#include "VayoRoot.h"
#include "VayoCamera.h"
#include "VayoRenderSystem.h"
#include "VayoSceneManager.h"

namespace
{
	Vayo::TouchDispatcher* gTouchDispatcher = NULL;
}

NS_VAYO_BEGIN

float Device::getAspectRatio() const
{
	return static_cast<float>(_screenSize._width) / _screenSize._height;
}

Device::Device()
	: _appPaused(false)
	, _minimized(false)
	, _maximized(false)
	, _resizing(false)
{
	gTouchDispatcher = Root::getSingleton().getTouchDispatcher();
}

void Device::onResize()
{
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	if (pRenderer && pRenderer->isActive())
	{
		pRenderer->setViewpot(Recti(0, 0, _screenSize._width, _screenSize._height));
		Camera* pActiveCamera = Root::getSingleton().getCurSceneMgr()->getActiveCamera();
		if (pActiveCamera)
		{
			float w = _screenSize._width;
			float h = _screenSize._height;
			if (0 == _screenSize._height)
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
	_wndCaption = wndCaption;
	const_cast<wstring&>(Root::getSingleton().getConfig().WndCaption) = _wndCaption;
}

void Device::setScreenSize(const Dimension2di& screenSize)
{
	_screenSize = screenSize;
	const_cast<Dimension2di&>(Root::getSingleton().getConfig().ScreenSize) = _screenSize;
}

void Device::injectMouseDown(EMouseKeys mouseKey, int x, int y)
{
	_mouseIsDown.addState(mouseKey);
	if (_mouseIsDown.checkState(EMK_LEFT))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(EMK_RIGHT))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_RIGHT);
	if (_mouseIsDown.checkState(EMK_MIDDLE))
		gTouchDispatcher->handleTouchBegan(x, y, EMK_MIDDLE);
}

void Device::injectMouseUp(EMouseKeys mouseKey, int x, int y)
{
	BitState mouseIsUp = mouseKey;
	if (_mouseIsDown.checkState(EMK_LEFT) && mouseIsUp.checkState(EMK_LEFT))
	{
		_mouseIsDown.eraseState(EMK_LEFT);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_LEFT);
	}
	if (_mouseIsDown.checkState(EMK_RIGHT) && mouseIsUp.checkState(EMK_RIGHT))
	{
		_mouseIsDown.eraseState(EMK_RIGHT);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_RIGHT);
	}
	if (_mouseIsDown.checkState(EMK_MIDDLE) && mouseIsUp.checkState(EMK_MIDDLE))
	{
		_mouseIsDown.eraseState(EMK_MIDDLE);
		gTouchDispatcher->handleTouchEnded(x, y, EMK_MIDDLE);
	}
}

void Device::injectMouseMove(int x, int y)
{
	gTouchDispatcher->setTouchCurPos(x, y);
	if (_mouseIsDown.checkState(EMK_LEFT))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_LEFT);
	if (_mouseIsDown.checkState(EMK_RIGHT))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_RIGHT);
	if (_mouseIsDown.checkState(EMK_MIDDLE))
		gTouchDispatcher->handleTouchMoved(x, y, EMK_MIDDLE);
}

void Device::injectMouseWheel(float wheel)
{
	gTouchDispatcher->handleTouchWheel(wheel);
}

void Device::injectKeyboard(unsigned int keyCode, unsigned int scanCode, bool keyDown)
{
}

void Device::injectPaint()
{
}

void Device::injectDestroy()
{
}

void Device::injectActivate()
{
	_appPaused = false;
	Root::getSingleton().getTimer().start();
	RenderSystem* pRenderer = Root::getSingleton().getActiveRenderer();
	if (pRenderer)
		pRenderer->restoreContext();
}

void Device::injectInactive()
{
	_appPaused = true;
	Root::getSingleton().getTimer().stop();
}

void Device::injectSizeMinimized(int w, int h)
{
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = true;
	_minimized = true;
	_maximized = false;
}

void Device::injectSizeMaximized(int w, int h)
{
	// Save the new client area dimensions.
	setScreenSize(Dimension2di(w, h));
	_appPaused = false;
	_minimized = false;
	_maximized = true;
	onResize();
}

void Device::injectSizeRestored(int w, int h)
{
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
	_appPaused = true;
	_resizing = true;
	Root::getSingleton().getTimer().stop();
}

void Device::injectExitSizeMove()
{
	_appPaused = false;
	_resizing = false;
	Root::getSingleton().getTimer().start();
	onResize();
}

void Device::injectInputLanguageChange()
{
}

NS_VAYO_END
