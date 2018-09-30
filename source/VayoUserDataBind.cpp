#include "VayoUserDataBind.h"

NS_VAYO_BEGIN

static Any s_emptyAny;

UserDataBind::UserDataBind(const UserDataBind& other)
{
	if (other._attributes)
		_attributes.reset(new Attributes(*other._attributes));
}

void UserDataBind::setUserData(const Any& anything)
{
	if (!_attributes)
		_attributes.reset(new Attributes());

	_attributes->_keylessAny = anything;
}

void UserDataBind::setUserData(const wstring& key, const Any& anything)
{
	if (!_attributes)
		_attributes.reset(new Attributes());

	if (!_attributes->_userObjectsMap)
		_attributes->_userObjectsMap.reset(new map<wstring, Any>());

	(*_attributes->_userObjectsMap)[key] = anything;
}

const Any& UserDataBind::getUserData(void) const
{
	if (!_attributes)
		return s_emptyAny;

	return _attributes->_keylessAny;
}

const Any& UserDataBind::getUserData(const wstring& key) const
{
	if (!_attributes)
		return s_emptyAny;

	if (!_attributes->_userObjectsMap)
		return s_emptyAny;

	map<wstring, Any>::iterator it = _attributes->_userObjectsMap->find(key);
	if (it != _attributes->_userObjectsMap->end())
		return it->second;

	return s_emptyAny;
}

void UserDataBind::eraseUserData(const wstring& key)
{
	if (_attributes && _attributes->_userObjectsMap)
	{
		map<wstring, Any>::iterator it = _attributes->_userObjectsMap->find(key);
		if (it != _attributes->_userObjectsMap->end())
			_attributes->_userObjectsMap->erase(it);
	}
}

void UserDataBind::clear()
{
	_attributes.reset();
}

NS_VAYO_END
