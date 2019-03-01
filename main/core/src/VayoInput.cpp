#include "VayoInput.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

bool TouchDelegate::isTouchEnabled() const
{
	return _touchEnabled;
}

TouchDelegate::TouchDelegate(const wstring& bindEvtID /*= L""*/)
	: _bindTouchEvtID(bindEvtID)
	, _captured(false)
	, _touchEnabled(false)
{
	if (_bindTouchEvtID.empty())
		_bindTouchEvtID = L"EDID_MAIN_DEVICE";
}

TouchDelegate::~TouchDelegate()
{
	enableTouch(false);
}

void TouchDelegate::enableTouch(bool enable, int priority /*= -1*/)
{
	if (_touchEnabled != enable)
	{
		_touchEnabled = enable;
		if (enable)
			TouchDispatcher::getSingleton().addTouchDelegate(this, priority);
		else
			TouchDispatcher::getSingleton().removeTouchDelegate(this);
	}
}

KeypadDelegate::KeypadDelegate(const wstring& bindEvtID /*= L""*/)
	: _bindKeypadEvtID(bindEvtID)
	, _keypadEnabled(false)
{
	if (_bindKeypadEvtID.empty())
		_bindKeypadEvtID = L"EDID_MAIN_DEVICE";
}

KeypadDelegate::~KeypadDelegate()
{
	enableKeypad(false);
}

bool KeypadDelegate::isKeypadEnabled() const
{
	return _keypadEnabled;
}

void KeypadDelegate::enableKeypad(bool enable, int priority /*= -1*/)
{
	if (_keypadEnabled != enable)
	{
		_keypadEnabled = enable;
		if (enable)
			KeypadDispatcher::getSingleton().addKeypadDelegate(this, priority);
		else
			KeypadDispatcher::getSingleton().removeKeypadDelegate(this);
	}
}

//-------------------------------------------------------------------------
// KeypadDispatcher class.
//-------------------------------------------------------------------------
KeypadDispatcher::KeypadDispatcher()
{
	_pKeypadDelegates = new map<wstring, list<KeypadDelegate*>>();
}

KeypadDispatcher::~KeypadDispatcher()
{
	delete _pKeypadDelegates;
}

void KeypadDispatcher::handleKeyClicked(const tagKeyInput& event)
{
	if (0 == _keypadActivateEvents.size())
		return;

	KeypadDelegate* pKeypadDelegate;
	list<KeypadDelegate*>* pKeypadDelegateList;
	list<KeypadDelegate*>::iterator itList;

	vector<list<KeypadDelegate*>*> activeEvts = _keypadActivateEvents;
	vector<list<KeypadDelegate*>*>::iterator itVec = activeEvts.begin();
	for (; itVec != activeEvts.end(); ++itVec)
	{
		pKeypadDelegateList = (*itVec);
		if (pKeypadDelegateList)
		{
			itList = pKeypadDelegateList->begin();
			for (; itList != pKeypadDelegateList->end(); ++itList)
			{
				pKeypadDelegate = (*itList);
				if (!pKeypadDelegate->_keypadEnabled)
					continue;

				if (pKeypadDelegate->keyClicked(event))
					break;
			}
		}
	}
}

void KeypadDispatcher::dispatchKeypadEvts(const vector<wstring>& evtIds)
{
	_keypadEvtIds = evtIds;
	clearDupElements(_keypadEvtIds);
	regActivateEvents();
}

void KeypadDispatcher::addKeypadDelegate(KeypadDelegate* pDelegate, int priority /*= -1*/)
{
	if (!pDelegate)
		return;

	map<wstring, list<KeypadDelegate*>>::iterator itmap = (*_pKeypadDelegates).find(pDelegate->_bindKeypadEvtID);
	if (itmap == (*_pKeypadDelegates).end())
	{
		list<KeypadDelegate*> touchList;
		itmap = (*_pKeypadDelegates).insert(make_pair(pDelegate->_bindKeypadEvtID, touchList)).first;
	}

	list<KeypadDelegate*>& keypadRefList = itmap->second;
	list<KeypadDelegate*>::iterator it = std::find(keypadRefList.begin(), keypadRefList.end(), pDelegate);
	if (it != keypadRefList.end())
		return;

	if (priority >= 0 && priority < keypadRefList.size())
	{
		it = keypadRefList.begin();
		while (priority--)
			++it;
		keypadRefList.insert(it, pDelegate);
	}
	else
		keypadRefList.push_back(pDelegate);

	regActivateEvents(pDelegate->_bindKeypadEvtID);
}

void KeypadDispatcher::removeKeypadDelegate(KeypadDelegate* pDelegate)
{
	if (!pDelegate)
		return;

	map<wstring, list<KeypadDelegate*>>::iterator itmap = (*_pKeypadDelegates).find(pDelegate->_bindKeypadEvtID);
	if (itmap == (*_pKeypadDelegates).end())
		return;

	list<KeypadDelegate*>& keypadRefList = itmap->second;
	list<KeypadDelegate*>::iterator it = std::find(keypadRefList.begin(), keypadRefList.end(), pDelegate);
	if (it != keypadRefList.end())
		keypadRefList.erase(it);

	if (keypadRefList.empty())
	{
		(*_pKeypadDelegates).erase(itmap);
		regActivateEvents(pDelegate->_bindKeypadEvtID);
	}
}

void KeypadDispatcher::regActivateEvents(const wstring& evtId /*= L""*/)
{
	vector<wstring>::const_iterator cit;
	if (evtId != L"")
	{
		cit = std::find(_keypadEvtIds.cbegin(), _keypadEvtIds.cend(), evtId);
		if (cit == _keypadEvtIds.end())
			return;
	}

	_keypadActivateEvents.clear();
	cit = _keypadEvtIds.cbegin();
	for (; cit != _keypadEvtIds.cend(); ++cit)
	{
		map<wstring, list<KeypadDelegate*>>::iterator itmap = (*_pKeypadDelegates).find(*cit);
		if (itmap != (*_pKeypadDelegates).end())
			_keypadActivateEvents.push_back(&itmap->second);
	}
}

//-------------------------------------------------------------------------
// TouchDispatcher class.
//-------------------------------------------------------------------------
TouchDispatcher::TouchDispatcher()
{
	_pTouchDelegates = new map<wstring, list<TouchDelegate*>>();
}

TouchDispatcher::~TouchDispatcher()
{
	delete _pTouchDelegates;
}

void TouchDispatcher::handleTouchBegan(int x, int y, EMouseKeys key)
{
	if (0 == _touchActivateEvents.size())
		return;

	int idx = 0;
	switch (key)
	{
	case EMK_RIGHT: idx = 1; break;
	case EMK_MIDDLE: idx = 2; break;
	}

	_curTouch[idx].reset();
	_curTouch[idx].setTouchPoint(x, y);

	TouchDelegate* pTouchDelegate;
	list<TouchDelegate*>* pTouchDelegateList;
	list<TouchDelegate*>::iterator itList;

	vector<list<TouchDelegate*>*> activeEvts = _touchActivateEvents;
	vector<list<TouchDelegate*>*>::iterator itVec = activeEvts.begin();
	for (; itVec != activeEvts.end(); ++itVec)
	{
		pTouchDelegateList = (*itVec);
		if (pTouchDelegateList)
		{
			itList = pTouchDelegateList->begin();
			for (; itList != pTouchDelegateList->end(); ++itList)
			{
				pTouchDelegate = (*itList);
				if (!pTouchDelegate->_touchEnabled)
					continue;

				pTouchDelegate->_captured = true;
				if (pTouchDelegate->touchBegan(_curTouch[idx], key))
					break;
			}
		}
	}
}

void TouchDispatcher::handleTouchMoved(int x, int y, EMouseKeys key)
{
	if (0 == _touchActivateEvents.size())
		return;

	int idx = 0;
	switch (key)
	{
	case EMK_RIGHT: idx = 1; break;
	case EMK_MIDDLE: idx = 2; break;
	}

	_curTouch[idx].setTouchPoint(x, y);

	TouchDelegate* pTouchDelegate;
	list<TouchDelegate*>* pTouchDelegateList;
	list<TouchDelegate*>::iterator itList;

	vector<list<TouchDelegate*>*> activeEvts = _touchActivateEvents;
	vector<list<TouchDelegate*>*>::iterator itVec = activeEvts.begin();
	for (; itVec != activeEvts.end(); ++itVec)
	{
		pTouchDelegateList = (*itVec);
		if (pTouchDelegateList)
		{
			itList = pTouchDelegateList->begin();
			for (; itList != pTouchDelegateList->end(); ++itList)
			{
				pTouchDelegate = (*itList);
				if (!pTouchDelegate->_touchEnabled)
					continue;

				if (!pTouchDelegate->_captured)
					continue;

				pTouchDelegate->touchMoved(_curTouch[idx], key);
			}
		}
	}
}

void TouchDispatcher::handleTouchEnded(int x, int y, EMouseKeys key)
{
	if (0 == _touchActivateEvents.size())
		return;

	int idx = 0;
	switch (key)
	{
	case EMK_RIGHT: idx = 1; break;
	case EMK_MIDDLE: idx = 2; break;
	}

	_curTouch[idx].setTouchPoint(x, y);

	TouchDelegate* pTouchDelegate;
	list<TouchDelegate*>* pTouchDelegateList;
	list<TouchDelegate*>::iterator itList;

	vector<list<TouchDelegate*>*> activeEvts = _touchActivateEvents;
	vector<list<TouchDelegate*>*>::iterator itVec = activeEvts.begin();
	for (; itVec != activeEvts.end(); ++itVec)
	{
		pTouchDelegateList = (*itVec);
		if (pTouchDelegateList)
		{
			itList = pTouchDelegateList->begin();
			for (; itList != pTouchDelegateList->end(); ++itList)
			{
				pTouchDelegate = (*itList);
				if (!pTouchDelegate->_touchEnabled)
					continue;

				if (!pTouchDelegate->_captured)
					continue;

				pTouchDelegate->touchEnded(_curTouch[idx], key);
				pTouchDelegate->_captured = false;
			}
		}
	}
}

void TouchDispatcher::handleTouchWheel(float wheel)
{
	if (0 == _touchActivateEvents.size())
		return;

	TouchDelegate* pTouchDelegate;
	list<TouchDelegate*>* pTouchDelegateList;
	list<TouchDelegate*>::iterator itList;

	vector<list<TouchDelegate*>*> activeEvts = _touchActivateEvents;
	vector<list<TouchDelegate*>*>::iterator itVec = activeEvts.begin();
	for (; itVec != activeEvts.end(); ++itVec)
	{
		pTouchDelegateList = (*itVec);
		if (pTouchDelegateList)
		{
			itList = pTouchDelegateList->begin();
			for (; itList != pTouchDelegateList->end(); ++itList)
			{
				pTouchDelegate = (*itList);
				if (!pTouchDelegate->_touchEnabled)
					continue;

				if (pTouchDelegate->touchWheel(_curPosition, wheel))
					break;
			}
		}
	}
}

void TouchDispatcher::setTouchCurPos(int x, int y)
{
	_curPosition.setTouchPoint(x, y);
}

void TouchDispatcher::dispatchTouchEvts(const vector<wstring>& evtIds)
{
	_touchEvtIds = evtIds;
	clearDupElements(_touchEvtIds);
	regActivateEvents();
}

void TouchDispatcher::addTouchDelegate(TouchDelegate* pDelegate, int priority /*= -1*/)
{
	if (!pDelegate)
		return;

	map<wstring, list<TouchDelegate*>>::iterator itmap = (*_pTouchDelegates).find(pDelegate->_bindTouchEvtID);
	if (itmap == (*_pTouchDelegates).end())
	{
		list<TouchDelegate*> touchList;
		itmap = (*_pTouchDelegates).insert(make_pair(pDelegate->_bindTouchEvtID, touchList)).first;
	}
	
	list<TouchDelegate*>& touchRefList = itmap->second;
	list<TouchDelegate*>::iterator it = std::find(touchRefList.begin(), touchRefList.end(), pDelegate);
	if (it != touchRefList.end())
		return;

	if (priority >= 0 && priority < touchRefList.size())
	{
		it = touchRefList.begin();
		while (priority--)
			++it;
		touchRefList.insert(it, pDelegate);
	}
	else
		touchRefList.push_back(pDelegate);

	regActivateEvents(pDelegate->_bindTouchEvtID);
}

void TouchDispatcher::removeTouchDelegate(TouchDelegate* pDelegate)
{
	if (!pDelegate)
		return;

	map<wstring, list<TouchDelegate*>>::iterator itmap = (*_pTouchDelegates).find(pDelegate->_bindTouchEvtID);
	if (itmap == (*_pTouchDelegates).end())
		return;

	list<TouchDelegate*>& touchRefList = itmap->second;
	list<TouchDelegate*>::iterator it = std::find(touchRefList.begin(), touchRefList.end(), pDelegate);
	if (it != touchRefList.end())
		touchRefList.erase(it);

	if (touchRefList.empty())
	{
		(*_pTouchDelegates).erase(itmap);
		regActivateEvents(pDelegate->_bindTouchEvtID);
	}
}

void TouchDispatcher::regActivateEvents(const wstring& evtId /*= L""*/)
{
	vector<wstring>::const_iterator cit;
	if (evtId != L"")
	{
		cit = std::find(_touchEvtIds.cbegin(), _touchEvtIds.cend(), evtId);
		if (cit == _touchEvtIds.end())
			return;
	}

	_touchActivateEvents.clear();
	cit = _touchEvtIds.cbegin();
	for (; cit != _touchEvtIds.cend(); ++cit)
	{
		map<wstring, list<TouchDelegate*>>::iterator itmap = (*_pTouchDelegates).find(*cit);
		if (itmap != (*_pTouchDelegates).end())
			_touchActivateEvents.push_back(&itmap->second);
	}
}

NS_VAYO_END
