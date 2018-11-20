/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 单例
\*************************************************************************/
#ifndef __VAYO_SINGLETON_H__
#define __VAYO_SINGLETON_H__

#include "VayoCommon.h"
NS_VAYO_BEGIN

//=========================================================================
// 单例类
//=========================================================================
template<typename T>
class Singleton
{
public:
	Singleton(void)
	{
		VAYO_ASSERT(!_singleton);
		_singleton = static_cast<T*>(this);
	}

	~Singleton(void)
	{
		VAYO_ASSERT(_singleton);
		_singleton = NULL;
	}

	static T& getSingleton(void)
	{
		VAYO_ASSERT(_singleton);
		return (*_singleton);
	}

	static T* getSingletonPtr(void)
	{
		return _singleton;
	}

private:
	static T* _singleton;
	DISALLOW_COPY_AND_ASSIGN(Singleton<T>)
};

template<typename T>
T* Singleton<T>::_singleton = NULL;

NS_VAYO_END

#endif // __VAYO_SINGLETON_H__
