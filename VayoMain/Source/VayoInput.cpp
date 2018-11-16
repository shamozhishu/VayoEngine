#include "VayoInput.h"

NS_VAYO_BEGIN

//-------------------------------------------------------------------------
// KeypadDispatcher class.
//-------------------------------------------------------------------------
KeypadDispatcher::~KeypadDispatcher()
{
	_keypadDelegates.clear();
}

void KeypadDispatcher::handleKeyClicked(const tagKeyInput& event)
{
	list<KeypadDelegate*>::iterator itor = _keypadDelegates.begin();
	for (; itor != _keypadDelegates.end(); itor++)
	{
		if ((*itor)->keyClicked(event))
			break;
	}
}

void KeypadDispatcher::addKeypadDelegate(KeypadDelegate* pDelegate, int priority /*= -1*/)
{
	if (NULL != pDelegate)
	{
		if (priority >= 0 && priority < _keypadDelegates.size())
		{
			list<KeypadDelegate*>::iterator it = _keypadDelegates.begin();
			while (priority--)
				++it;
			_keypadDelegates.insert(it, pDelegate);
		}
		else
			_keypadDelegates.push_back(pDelegate);
	}
}

void KeypadDispatcher::removeKeypadDelegate(KeypadDelegate* pDelegate)
{
	if (NULL == pDelegate)
		return;
	list<KeypadDelegate*>::iterator itor = std::find(_keypadDelegates.begin(), _keypadDelegates.end(), pDelegate);
	if (itor != _keypadDelegates.end())
	{
		_keypadDelegates.erase(itor);
	}
}

//-------------------------------------------------------------------------
// TouchDispatcher class.
//-------------------------------------------------------------------------
TouchDispatcher::~TouchDispatcher()
{
	_touchDelegates.clear();
}

void TouchDispatcher::handleTouchBegan(int x, int y, EMouseKeys key)
{
	if (0 == _touchDelegates.size())
		return;
	_curTouch[key].reset();
	_curTouch[key].setTouchPoint(x, y);
	list<TouchDelegate*>::iterator itor = _touchDelegates.begin();
	for (; itor != _touchDelegates.end(); itor++)
	{
		TouchDelegate* pTouchDelegate = (*itor);
		pTouchDelegate->_captured = true;
		if (pTouchDelegate->touchBegan(_curTouch[key], key))
			break;
	}
}

void TouchDispatcher::handleTouchMoved(int x, int y, EMouseKeys key)
{
	if (0 == _touchDelegates.size())
		return;
	_curTouch[key].setTouchPoint(x, y);
	list<TouchDelegate*>::iterator itor = _touchDelegates.begin();
	for (; itor != _touchDelegates.end(); itor++)
	{
		TouchDelegate* pTouchDelegate = (*itor);
		if (!pTouchDelegate->_captured)
			continue;
		pTouchDelegate->touchMoved(_curTouch[key], key);
	}
}

void TouchDispatcher::handleTouchEnded(int x, int y, EMouseKeys key)
{
	if (0 == _touchDelegates.size())
		return;
	_curTouch[key].setTouchPoint(x, y);
	list<TouchDelegate*>::iterator itor = _touchDelegates.begin();
	for (; itor != _touchDelegates.end(); itor++)
	{
		TouchDelegate* pTouchDelegate = (*itor);
		if (!pTouchDelegate->_captured)
			continue;
		pTouchDelegate->touchEnded(_curTouch[key], key);
		pTouchDelegate->_captured = false;
	}
}

void TouchDispatcher::handleTouchWheel(float wheel)
{
	list<TouchDelegate*>::iterator itor = _touchDelegates.begin();
	for (; itor != _touchDelegates.end(); itor++)
	{
		if ((*itor)->touchWheel(_curPosition, wheel))
			break;
	}
}

void TouchDispatcher::setTouchCurPos(int x, int y)
{
	_curPosition.setTouchPoint(x, y);
}

void TouchDispatcher::addTouchDelegate(TouchDelegate* pDelegate, int priority /*= -1*/)
{
	if (NULL != pDelegate)
	{
		if (priority >= 0 && priority < _touchDelegates.size())
		{
			list<TouchDelegate*>::iterator it = _touchDelegates.begin();
			while (priority--)
				++it;
			_touchDelegates.insert(it, pDelegate);
		}
		else
			_touchDelegates.push_back(pDelegate);
	}
}

void TouchDispatcher::removeTouchDelegate(TouchDelegate* pDelegate)
{
	if (NULL == pDelegate)
		return;
	list<TouchDelegate*>::iterator itor = std::find(_touchDelegates.begin(), _touchDelegates.end(), pDelegate);
	if (itor != _touchDelegates.end())
	{
		_touchDelegates.erase(itor);
	}
}

NS_VAYO_END
