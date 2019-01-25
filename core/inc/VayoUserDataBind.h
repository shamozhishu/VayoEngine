/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 用户数据绑定(建立起用户数据对象和Vayo引擎对象之间的连接)
\*************************************************************************/
#ifndef __VAYO_USER_DATA_BIND_H__
#define __VAYO_USER_DATA_BIND_H__

#include "VayoAny.h"
NS_VAYO_BEGIN

class _VayoExport UserDataBind
{
public:
	UserDataBind() {}
	UserDataBind(const UserDataBind& other);
	UserDataBind& swap(UserDataBind& rhs)
	{
		std::swap(_attributes, rhs._attributes);
		return *this;
	}
	UserDataBind& operator=(const UserDataBind& rhs)
	{
		UserDataBind(rhs).swap(*this);
		return *this;
	}
	inline friend wstringstream& operator<<(wstringstream& ss, const UserDataBind& in)
	{
		return in.operator<<(ss);
	}
	inline friend wstringstream& operator>>(wstringstream& ss, UserDataBind& out)
	{
		return out.operator>>(ss);
	}
	wstringstream& operator<<(wstringstream& ss) const;
	wstringstream& operator>>(wstringstream& ss);

public:
	void setUserData(const Any& anything);
	void setUserData(const wstring& key, const Any& anything);
	const Any& getUserData(void) const;
	const Any& getUserData(const wstring& key) const;
	void eraseUserData(const wstring& key);
	void clear();

protected:
	struct Attributes
	{
		Attributes() {}
		Attributes(const Attributes& other) : _keylessAny(other._keylessAny)
		{
			if (other._userObjectsMap)
				_userObjectsMap.reset(new map<wstring, Any>(*other._userObjectsMap));
		}

		Any                                _keylessAny;
		std::unique_ptr<map<wstring, Any>> _userObjectsMap;
	};

	const Attributes* getAttributes() const { return _attributes.get(); }
	Attributes* getAttributes() { return _attributes.get(); }

private:
	std::unique_ptr<Attributes> _attributes;
};

NS_VAYO_END

#endif // __VAYO_USER_DATA_BIND_H__
