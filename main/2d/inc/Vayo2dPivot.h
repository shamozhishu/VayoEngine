/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 2D引擎枢纽
\*************************************************************************/
#ifndef __VAYO2D_PIVOT_H__
#define __VAYO2D_PIVOT_H__

#include "Vayo2dSupport.h"
#include "VayoSingleton.h"
#include "VayoTimer.h"
#include "VayoCore.h"
#include "VayoPluginSystem.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Pivot : public Singleton<Pivot>, public Core, public PluginSystem
{
public:
	Pivot();
	~Pivot();
	bool launch(Core::Config* config);
	void resize(Device* dev = NULL);
	void activate(Device* dev = NULL);
	bool renderOneFrame(Device* renderWnd = NULL);
	void bootFrame(Device* dev, const wstring& scenename = L"", const wstring& userEvtID = L"");
	bool openUI(Device* dev = NULL);
	void closeUI(Device* dev = NULL);
	bool setPixelFormat(Device* dev = NULL);

protected:
	PROPERTY_RW(Renderer*, _renderer, Renderer)
};

NS_VAYO2D_END

#endif // __VAYO2D_PIVOT_H__
