#include "Vayo2dLayerManager.h"
#include "Vayo2dLayer.h"
#include "Vayo2dWatcher.h"

NS_VAYO2D_BEGIN

void LayerManager::updateGraphicQueue()
{
	_graphicQueues.update();
}

LayerManager::LayerManager(const wstring& layerMgrName)
	: _graphicQueues(this)
	, _name(layerMgrName)
	, _activeWatcher(nullptr)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"LayerManager" << idx;
		++idx;
		_name = ss.str();
	}

	_rootLayer = new Layer(_name + L"_RootLayer", NULL, this);
}

LayerManager::~LayerManager()
{
	destroyAllBodies();
	destroyAllLayers();
	SAFE_DELETE(_rootLayer);
}

void LayerManager::setActiveWatcher(Watcher* pActiveWatcher)
{
	if ((pActiveWatcher && pActiveWatcher->getOriLayerMgr() != this) || pActiveWatcher == _activeWatcher)
		return;
	if (_activeWatcher)
	{
		_activeWatcher->enableTouch(false);
		_activeWatcher->enableKeypad(false);
	}
	_activeWatcher = pActiveWatcher;
	if (_activeWatcher)
	{
		_activeWatcher->enableTouch(true);
		_activeWatcher->enableKeypad(true);
	}
}

bool LayerManager::isCulled(Layer* layer) const
{
	bool result = false;
	if (!_activeWatcher || !layer->isAutomaticCulling())
		return result;

	result = false;
	return result;
}

bool LayerManager::registerForRendering(Graphics* pGraph, unsigned int queueID /*= EGQ_MAIN_BODY*/)
{
	if (nullptr == pGraph)
		return false;
	_graphicQueues.addGraphics(pGraph, queueID);
	return true;
}

void LayerManager::showAllWireBoundingAreas(bool bShow)
{
	map<wstring, Layer*>::iterator it = _layersPool.begin();
	for (; it != _layersPool.end(); ++it)
		it->second->showWireBoundingArea(bShow);
}

bool LayerManager::loadLayerset(const wstring& layersetFile)
{
	return true;
}

bool LayerManager::saveLayerset(const wstring& layersetFile)
{
	return true;
}

void LayerManager::destroyLayer(const wstring& name, bool cleanUpChildren /*= false*/)
{
	Layer* layer = nullptr;
	map<wstring, Layer*>::iterator it = _layersPool.find(name);
	if (it != _layersPool.end())
	{
		layer = it->second;
		_layersPool.erase(it);

		if (cleanUpChildren)
		{
			list<Joint*> nodes = layer->getChildren();
			list<Joint*>::const_iterator cit = nodes.cbegin();
			for (; cit != nodes.cend(); ++cit)
			{
				Layer* pChild = dynamic_cast<Layer*>(*cit);
				if (pChild)
					destroyLayer(pChild->getName(), cleanUpChildren);
			}

			map<wstring, Body*> bodies = layer->getBodies();
			map<wstring, Body*>::const_iterator itmap = bodies.cbegin();
			for (; itmap != bodies.cend(); ++itmap)
			{
				Body* pBody = itmap->second;
				destroyBody(pBody->getName());
			}
		}

		delete layer;
	}
}

void LayerManager::destroyLayer(Layer* layer, bool cleanUpChildren /*= false*/)
{
	if (layer)
		destroyLayer(layer->getName(), cleanUpChildren);
}

void LayerManager::destroyAllLayers()
{
	Layer* layer = nullptr;
	map<wstring, Layer*>::iterator it = _layersPool.begin();
	for (; it != _layersPool.end(); ++it)
	{
		layer = it->second;
		delete layer;
	}
	_layersPool.clear();
}

void LayerManager::destroyBody(const wstring& name)
{
	Body* body = nullptr;
	map<wstring, Body*>::iterator it = _bodiesPool.find(name);
	if (it != _bodiesPool.end())
	{
		body = it->second;
		delete body;
		_bodiesPool.erase(it);
	}
}

void LayerManager::destroyBody(Body* body)
{
	if (body)
		destroyBody(body->getName());
}

void LayerManager::destroyAllBodies()
{
	Body* body = nullptr;
	map<wstring, Body*>::iterator it = _bodiesPool.begin();
	for (; it != _bodiesPool.end(); ++it)
	{
		body = it->second;
		delete body;
	}
	_bodiesPool.clear();
}

bool LayerManager::recursionLoading(XMLElement* element, Layer* parent)
{
	return true;
}

void LayerManager::recursionSaving(XMLElement* element, Layer* parent, tinyxml2::XMLDocument& document)
{

}

NS_VAYO2D_END
