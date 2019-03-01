/*************************************************************************\
* Õ˚”—“˝«Ê1.0
* Copyright (c) 2018-2019 authored by ÷Ïº”∫∆
* 2D“˝«Ê ‡≈¶
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
	void resize(Device* dev = nullptr);
	bool fireFrameRendering(Device* renderWnd = nullptr);
	void bootFrame(Device* dev, const wstring& layermgrname = L"", const wstring& userEvtID = L"");
	void addRenderer(Renderer* newRenderer);

	LayerManager* createLayerMgr(const wstring& layermgrName = L"");
	LayerManager* findLayerMgr(const wstring& layermgrName);
	void          destroyLayerMgr(const wstring& layermgrName);
	void          destroyLayerMgr(LayerManager* layermgr);
	void          destroyAllLayerMgrs();
	void          setNullCurLayerMgr();

private:
	map<wstring, Renderer*> _renderers;
	map<wstring, LayerManager*> _layerMgrPool;

protected:
	PROPERTY_R(LayerManager*,   _curLayerMgr,    CurLayerMgr)
	PROPERTY_R(Renderer*,       _activeRenderer, ActiveRenderer)
	PROPERTY_R(SurfaceManager*, _surfaceManager, SurfaceManager)
	PROPERTY_R(FeatureManager*, _featureManager, FeatureManager)
};

NS_VAYO2D_END

#endif // __VAYO2D_PIVOT_H__
