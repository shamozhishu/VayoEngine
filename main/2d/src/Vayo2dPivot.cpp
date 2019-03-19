#include "Vayo2dPivot.h"
#include "Vayo2dLayer.h"
#include "Vayo2dWatcher.h"
#include "Vayo2dRenderer.h"
#include "VayoConfigManager.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dPictureManager.h"
#include "Vayo2dFeatureManager.h"

NS_VAYO2D_BEGIN

void Pivot::addRenderer(Renderer* newRenderer)
{
	if (newRenderer)
		_renderers[newRenderer->getName()] = newRenderer;
}

Pivot::Pivot()
	: _curLayerMgr(nullptr)
	, _activeRenderer(nullptr)
{
	_pictureManager = new PictureManager();
	_featureManager = new FeatureManager();
}

Pivot::~Pivot()
{
	destroyAllLayerMgrs();
	SAFE_DELETE(_featureManager);
	SAFE_DELETE(_pictureManager);
	unloadPlugins();
}

bool Pivot::launch(Core::Config* config)
{
	do 
	{
		IF_FALSE_BREAK(config);
		IF_FALSE_BREAK(ConfigManager::getSingleton().init(config->RootDirectory, _2D));
		IF_FALSE_BREAK(Core::launch(config));
		loadPlugins(_2D);
		map<wstring, Renderer*>::iterator it = _renderers.find(config->RendererName);
		if (it != _renderers.end())
			_activeRenderer = it->second;
		IF_FALSE_BREAK(_activeRenderer);
		IF_FALSE_BREAK(_activeRenderer->init());
		IF_FALSE_BREAK(_pictureManager && _pictureManager->init());
		IF_FALSE_BREAK(_featureManager && _featureManager->init());
		if (config->MainDeviceAttrib.TurnOnUI)
			openUI(_mainDevice);
		_isLaunched = true;
		return true;
	} while (0);
	_isLaunched = false;
	return false;
}

void Pivot::resize(Device* dev /*= nullptr*/)
{
	if (!dev)
		dev = _activeDevice;
	if (!dev)
		dev = _mainDevice;

	if (_activeRenderer)
		_activeRenderer->resize(dev->getAttrib().ScreenSize._width, dev->getAttrib().ScreenSize._height);

	if (_curLayerMgr)
	{
		Watcher* pActiveWatcher = _curLayerMgr->getActiveWatcher();
		if (pActiveWatcher)
		{
			Position2df cecterPos = Position2df(dev->getScreenSize()._width / 2.0f, dev->getScreenSize()._height / 2.0f);
			float aspect = pActiveWatcher->getWindow().getHeight() / pActiveWatcher->getWindow().getWidth();
			float w = pActiveWatcher->getViewport().getWidth();
			float h = w * aspect;
			float halfW = w * 0.5f;
			float halfH = h * 0.5f;
			pActiveWatcher->setViewport(Rectf(cecterPos._x - halfW, cecterPos._y - halfH, cecterPos._x + halfW, cecterPos._y + halfH));
		}
	}
}

bool Pivot::fireFrameRendering(Device* renderWnd /*= nullptr*/)
{
	if (!renderWnd)
		renderWnd = _activeDevice;

	if (!renderWnd)
		return false;

	if (!_activeRenderer->beginDraw(renderWnd))
		return false;

	if (_curLayerMgr && _curLayerMgr->getActiveWatcher())
	{
		_curLayerMgr->getActiveWatcher()->refresh();
		_curLayerMgr->getRootLayer()->visit(_timer.deltaTime());
		_curLayerMgr->updateGraphicQueue();
	}

	if (!Core::fireFrameRendering(renderWnd))
		return false;

	return _activeRenderer->endDraw();
}

void Pivot::bootFrame(Device* dev, const wstring& layermgrname /*= L""*/, const wstring& userEvtID /*= L""*/)
{
	LayerManager* layerMgr = nullptr;
	if (!layermgrname.empty())
		layerMgr = findLayerMgr(layermgrname);

	bool isDispatchEvts = false;
	if (layerMgr && _curLayerMgr != layerMgr)
	{
		isDispatchEvts = true;
		_curLayerMgr = layerMgr;
	}

	if (dev && _activeDevice != dev)
	{
		isDispatchEvts = true;
		_activeDevice = dev;
	}

	if (!userEvtID.empty())
		isDispatchEvts = true;

	if (isDispatchEvts)
	{
		vector<wstring> evtIds;
		if (_activeDevice)
			evtIds.push_back(_activeDevice->getDeviceCode());
		if (_curLayerMgr)
			evtIds.push_back(_curLayerMgr->getName());
		if (!userEvtID.empty())
			evtIds.push_back(userEvtID);
		TouchDispatcher::getSingleton().dispatchTouchEvts(evtIds);
		KeypadDispatcher::getSingleton().dispatchKeypadEvts(evtIds);
	}
}

LayerManager* Pivot::createLayerMgr(const wstring& layermgrName /*= L""*/)
{
	LayerManager* ret = new LayerManager(layermgrName);
	if (!ret)
		return nullptr;

	LayerManager* pFindedLayerMgr = findLayerMgr(ret->getName());
	if (pFindedLayerMgr)
		destroyLayerMgr(pFindedLayerMgr);

	_layerMgrPool[ret->getName()] = ret;
	return ret;
}

LayerManager* Pivot::findLayerMgr(const wstring& layermgrName)
{
	map<wstring, LayerManager*>::iterator it = _layerMgrPool.find(layermgrName);
	if (it != _layerMgrPool.end())
		return it->second;
	return nullptr;
}

void Pivot::destroyLayerMgr(const wstring& layermgrName)
{
	LayerManager* pLayerMgr;
	map<wstring, LayerManager*>::iterator it = _layerMgrPool.find(layermgrName);
	if (it != _layerMgrPool.end())
	{
		pLayerMgr = it->second;
		if (pLayerMgr == _curLayerMgr)
			_curLayerMgr = nullptr;
		delete pLayerMgr;
		_layerMgrPool.erase(it);
	}
}

void Pivot::destroyLayerMgr(LayerManager* layermgr)
{
	if (layermgr)
		destroyLayerMgr(layermgr->getName());
}

void Pivot::destroyAllLayerMgrs()
{
	map<wstring, LayerManager*>::iterator it = _layerMgrPool.begin();
	for (; it != _layerMgrPool.end(); ++it)
		delete it->second;
	_curLayerMgr = nullptr;
	_layerMgrPool.clear();
}

void Pivot::setNullCurLayerMgr()
{
	_curLayerMgr = nullptr;
}

NS_VAYO2D_END
