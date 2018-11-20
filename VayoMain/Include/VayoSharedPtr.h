/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 智能指针
\*************************************************************************/
#ifndef __VAYO_SHARED_PTR_H__
#define __VAYO_SHARED_PTR_H__

#include "VayoCommon.h"
NS_VAYO_BEGIN

//=========================================================================
// 引用计数型智能指针类
//=========================================================================
template<typename T>
class SharedPtr
{
public:
	typedef T EleType;

private:
	class RefCnt
	{
	public:
		RefCnt() : _ref(new int(1)) {}

		RefCnt(const RefCnt& rhs) : _ref(rhs._ref) { ++*_ref; }

		~RefCnt()
		{
			if (--*_ref == 0)
				delete _ref;
		}

		bool only()
		{
			return *_ref == 1;
		}

		bool reattach(const RefCnt& rhs)
		{
			++*rhs._ref;
			if (--*_ref == 0)
			{
				SAFE_DELETE(_ref);
				_ref = rhs._ref;
				return true;
			}
			_ref = rhs._ref;
			return false;
		}

	private:
		int* _ref;
		DISALLOW_ASSIGN(RefCnt)
	};

public:
	SharedPtr() : _p(NULL) {}

	SharedPtr(EleType* p) : _p(p) {}

	SharedPtr(const SharedPtr& rhs) : _p(rhs._p), _u(rhs._u) {}

	SharedPtr& operator=(const SharedPtr& rhs)
	{
		if (_u.reattach(rhs._u))
			SAFE_DELETE(_p);
		_p = rhs._p;
		return *this;
	}

	~SharedPtr()
	{
		if (_u.only())
			SAFE_DELETE(_p);
	}

	EleType& operator*() const
	{
		VAYO_ASSERT(_p);
		return *_p;
	}

	EleType* operator->() const
	{
		VAYO_ASSERT(_p);
		return _p;
	}

	operator bool() const
	{
		return _p != NULL;
	}

	EleType* get() const
	{
		return _p;
	}

private:
	EleType* _p;
	RefCnt _u;
};

template<typename T, typename U>
SharedPtr<T> dynamic_ptr_cast(const SharedPtr<U>& rhs)
{
	T* p = dynamic_cast<T*>(rhs.get());
	return p ? SharedPtr<T>(p, rhs._u) : SharedPtr<T>();
}

NS_VAYO_END

#endif // __VAYO_SHARED_PTR_H__
