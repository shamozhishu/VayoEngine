/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 层管理器
\*************************************************************************/
#ifndef __VAYO2D_LAYER_MANAGER_H__
#define __VAYO2D_LAYER_MANAGER_H__

#include "Vayo2dSupport.h"
#include "Vayo2dGraphicQueue.h"
#include "VayoAttribSerializer.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport LayerManager
{
public:
	LayerManager(const wstring& layerMgrName);
	~LayerManager();
	void updateGraphicQueue();
	void setActiveWatcher(Watcher* pActiveWatcher);
	bool isCulled(Layer* layer) const;
	bool registerForRendering(Graphics* pGraph, unsigned int queueID = EGQ_MAIN_BODY);
	void showAllWireBoundingRects(bool bShow);

	bool loadLayerset(const wstring& layersetFile);
	bool saveLayerset(const wstring& layersetFile);

	template<typename T> T* createLayer(Layer* parent, const wstring& name = L"");
	template<typename T> T* findLayer(const wstring& name);
	void                    destroyLayer(const wstring& name, bool cleanUpChildren = false);
	void                    destroyLayer(Layer* layer, bool cleanUpChildren = false);
	void                    destroyAllLayers();

	template<typename T> T* createBody(const wstring& name = L"");
	template<typename T> T* findBody(const wstring& name);
	void                    destroyBody(const wstring& name);
	void                    destroyBody(Body* body);
	void                    destroyAllBodies();

	template<typename T> T* createAction(const wstring& name = L"");
	template<typename T> T* findAction(const wstring& name);
	void                    destroyAction(const wstring& name);
	void                    destroyAction(Action* action);
	void                    destroyAllActions();

private:
	bool recursionLoading(XMLElement* element, Layer* parent);
	void recursionSaving(XMLElement* element, Layer* parent, tinyxml2::XMLDocument& document);
	PROPERTY_R(wstring, _name, Name)
	PROPERTY_R(Layer*, _rootLayer, RootLayer)
	PROPERTY_R(Watcher*, _activeWatcher, ActiveWatcher)
	DISALLOW_COPY_AND_ASSIGN(LayerManager)

private:
	GraphicQueueGroup     _graphicQueues;
	map<wstring, Layer*>  _layersPool;
	map<wstring, Body*>   _bodiesPool;
	map<wstring, Action*> _actionsPool;
};

template<typename T>
T* LayerManager::createLayer(Layer* parent, const wstring& name /*= L""*/)
{
	T* ret = new T(name, parent, this);
	Layer* pLayer = dynamic_cast<Layer*>(ret);
	if (NULL == pLayer)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	Layer* pFindedLayer = findLayer<Layer>(pLayer->getName());
	if (pFindedLayer)
	{
		SAFE_DELETE(pFindedLayer);
	}

	_layersPool[pLayer->getName()] = pLayer;
	return ret;
}

template<typename T>
T* LayerManager::findLayer(const wstring& name)
{
	T* ret = NULL;
	map<wstring, Layer*>::iterator it = _layersPool.find(name);
	if (it != _layersPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

template<typename T>
T* LayerManager::createBody(const wstring& name /*= L""*/)
{
	T* ret = new T(name, this);
	Body* pBody = dynamic_cast<Body*>(ret);
	if (NULL == pBody)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	Body* pFindedBody = findBody<Body>(pBody->getName());
	if (pFindedBody)
	{
		SAFE_DELETE(pFindedBody);
	}

	_bodiesPool[pBody->getName()] = pBody;
	return ret;
}

template<typename T>
T* LayerManager::findBody(const wstring& name)
{
	T* ret = NULL;
	map<wstring, Body*>::iterator it = _bodiesPool.find(name);
	if (it != _bodiesPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

template<typename T>
T* LayerManager::createAction(const wstring& name /*= L""*/)
{
	T* ret = new T(name, this);
	Action* pAct = dynamic_cast<Action*>(ret);
	if (NULL == pAct)
	{
		SAFE_DELETE(ret);
		return NULL;
	}

	Action* pFindedAct = findAction<Action>(pAct->getName());
	if (pFindedAct)
	{
		SAFE_DELETE(pFindedAct);
	}

	_actionsPool[pAct->getName()] = pAct;
	return ret;
}

template<typename T>
T* LayerManager::findAction(const wstring& name)
{
	T* ret = NULL;
	map<wstring, Action*>::iterator it = _actionsPool.find(name);
	if (it != _actionsPool.end())
		ret = dynamic_cast<T*>(it->second);
	return ret;
}

NS_VAYO2D_END

#endif // __VAYO2D_LAYER_MANAGER_H__
