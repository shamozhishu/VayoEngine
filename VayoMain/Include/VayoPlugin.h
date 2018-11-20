/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 插件
\*************************************************************************/
#ifndef __VAYO_PLUGIN_H__
#define __VAYO_PLUGIN_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport Plugin
{
public:
	Plugin() {}
	virtual ~Plugin() {}
	virtual const wstring& getName() const = 0;
	virtual void install() = 0;
	virtual void initialise() = 0;
	virtual void shutdown() = 0;
	virtual void uninstall() = 0;
};

NS_VAYO_END

#endif // __VAYO_PLUGIN_H__
