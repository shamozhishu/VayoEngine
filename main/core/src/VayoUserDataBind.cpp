#include "VayoUserDataBind.h"
#include "math/VayoVector2d.h"
#include "math/VayoVector3d.h"

NS_VAYO_BEGIN

static Any s_emptyAny;

static bool putInUserDataType(wstringstream& ss, const type_info& typeinfo)
{
	bool isPutin = true;
	if (typeinfo == typeid(bool))
		ss << L"bool( ";
	else if (typeinfo == typeid(int))
		ss << L"int( ";
	else if (typeinfo == typeid(unsigned int))
		ss << L"uint( ";
	else if (typeinfo == typeid(short))
		ss << L"short( ";
	else if (typeinfo == typeid(unsigned short))
		ss << L"ushort( ";
	else if (typeinfo == typeid(long))
		ss << L"long( ";
	else if (typeinfo == typeid(unsigned long))
		ss << L"ulong( ";
	else if (typeinfo == typeid(long long))
		ss << L"longlong( ";
	else if (typeinfo == typeid(unsigned long long))
		ss << L"ulonglong( ";
	else if (typeinfo == typeid(float))
		ss << L"float( ";
	else if (typeinfo == typeid(double))
		ss << L"double( ";
	else if (typeinfo == typeid(wstring))
		ss << L"wstring( ";
	else if (typeinfo == typeid(Vector2df))
		ss << L"Vector2df( ";
	else if (typeinfo == typeid(Vector2di))
		ss << L"Vector2di( ";
	else if (typeinfo == typeid(Vector3df))
		ss << L"Vector3df( ";
	else if (typeinfo == typeid(Vector3di))
		ss << L"Vector3di( ";
	else
		isPutin = false;
	return isPutin;
}

static bool putOutUserDataType(wstringstream& ss, wstring& tag, Any& data)
{
	bool isPutout = true;
	wstring ignore;
	ss >> tag;
	if (tag == L"bool(")
	{
		bool var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"int(")
	{
		int var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"uint(")
	{
		unsigned int var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"short(")
	{
		short var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"ushort(")
	{
		unsigned short var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"long(")
	{
		long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"ulong(")
	{
		unsigned long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"longlong(")
	{
		long long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"ulonglong(")
	{
		unsigned long long var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"float(")
	{
		float var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"double(")
	{
		double var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"wstring(")
	{
		wstring var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"Vector2df(")
	{
		Vector2df var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"Vector2di(")
	{
		Vector2di var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"Vector3df(")
	{
		Vector3df var;
		ss >> var >> ignore;
		data = var;
	}
	else if (tag == L"Vector3di(")
	{
		Vector3di var;
		ss >> var >> ignore;
		data = var;
	}
	else
		isPutout = false;
	return isPutout;
}

UserDataBind::UserDataBind(const UserDataBind& other)
{
	if (other._attributes)
		_attributes.reset(new Attributes(*other._attributes));
}

wstringstream& UserDataBind::operator<<(wstringstream& ss) const
{
	if (_attributes)
	{
		ss << L"UserData{ ";
		if (putInUserDataType(ss, _attributes->_keylessAny.type()))
			ss << _attributes->_keylessAny << L") ";
		
		if (_attributes->_userObjectsMap)
		{
			ss << L"[ " << (int)(_attributes->_userObjectsMap->size()) << L" ";
			map<wstring, Any>::iterator it = _attributes->_userObjectsMap->begin();
			for (; it != _attributes->_userObjectsMap->end(); ++it)
			{
				if (putInUserDataType(ss, it->second.type()))
				{
					ss << it->second << L") ";
					ss << L": " << it->first << L" ";
				}
			}
			ss << L"] ";
		}
		ss << L"}";
	}
	return ss;
}

wstringstream& UserDataBind::operator>>(wstringstream& ss)
{
	wstring tag;
	ss >> tag;
	if (tag != L"UserData{")
		return ss;

	if (!_attributes)
		_attributes.reset(new UserDataBind::Attributes());

	if (putOutUserDataType(ss, tag, _attributes->_keylessAny))
		ss >> tag;

	if (tag == L"}")
		return ss;

	if (tag == L"[")
	{
		int size;
		ss >> size;
		if (size > 0)
		{
			if (!_attributes->_userObjectsMap)
				_attributes->_userObjectsMap.reset(new map<wstring, Any>());

			Any keyValue;
			for (int i = 0; i < size; ++i)
			{
				if (putOutUserDataType(ss, tag, keyValue))
				{
					ss >> tag >> tag;
					(*_attributes->_userObjectsMap)[tag] = keyValue;
				}
			}
		}
		ss >> tag >> tag;
	}

	return ss;
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
