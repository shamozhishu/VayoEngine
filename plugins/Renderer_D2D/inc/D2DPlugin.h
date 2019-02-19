/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Direct2D插件
\*************************************************************************/
#ifndef __D2D_PLUGIN_H__
#define __D2D_PLUGIN_H__

#include "D2DSupport.h"
#include "VayoPlugin.h"

class D2DPlugin : public Plugin
{
public:
	D2DPlugin();
	const wstring& getName() const;
	void install();
	void initialise();
	void shutdown();
	void uninstall();

protected:
	D2DRenderer* _renderer;
};

#endif // __D2D_PLUGIN_H__
