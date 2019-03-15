#include "Vayo2dRenderer.h"
#include "VayoCore.h"

NS_VAYO2D_BEGIN

bool Renderer::isDeviceLost(EDeviceID devid /*= EDID_CURRENT_DEV*/) const
{
	if (devid == EDID_CURRENT_DEV)
	{
		Device* dev = Core::getCore().getActiveDevice();
		if (dev)
			devid = dev->getDeviceID();
		else
			devid = EDID_MAIN_DEVICE;
	}
	return _deviceLost[devid];
}

Renderer::Renderer(const wstring& name)
	: _name(name)
	, _renderTarget(ERT_WINDOW_DEFAULT)
{
	for (unsigned int i = EDID_AUX_DEVICE0; i < EDID_DEVICE_COUNT; ++i)
		_deviceLost[i] = false;
}

Renderer::~Renderer()
{
}

Geometry* Renderer::findGeometry(const wstring& name)
{
	map<wstring, Geometry*>::iterator it = _geometries.find(name);
	if (it != _geometries.end())
		return it->second;
	return nullptr;
}

void Renderer::destroyGeometry(const wstring& name)
{
	map<wstring, Geometry*>::iterator it = _geometries.find(name);
	if (it != _geometries.end())
	{
		SAFE_DELETE(it->second);
		_geometries.erase(it);
	}
}

void Renderer::destroyAllGeometries()
{
	map<wstring, Geometry*>::iterator it = _geometries.begin();
	for (; it != _geometries.end(); ++it)
	{
		SAFE_DELETE(it->second);
	}
	_geometries.clear();
}

Paintbrush* Renderer::getPaintbrush(EDeviceID devid /*= EDID_CURRENT_DEV*/)
{
	Paintbrush* pPaintbrush = nullptr;
	if (devid == EDID_CURRENT_DEV)
	{
		Device* dev = Core::getCore().getActiveDevice();
		if (dev)
			devid = dev->getDeviceID();
		else
			devid = EDID_MAIN_DEVICE;
	}

	switch (_renderTarget)
	{
	case ERT_NONE:
		return nullptr;
	case ERT_WINDOW_DEFAULT:
	case ERT_WINDOW_SOFTWARE:
	case ERT_WINDOW_HARDWARE:
		pPaintbrush = _hwndPaintbrushs[devid].get();
		if (!pPaintbrush)
		{
			_hwndPaintbrushs[devid] = createPaintbrush(_renderTarget, devid);
			pPaintbrush = _hwndPaintbrushs[devid].get();
		}
		break;
	case ERT_MEMORY:
		pPaintbrush = _bitmapPaintbrushs[devid].get();
		if (!pPaintbrush)
		{
			_bitmapPaintbrushs[devid] = createPaintbrush(ERT_MEMORY, devid);
			pPaintbrush = _bitmapPaintbrushs[devid].get();
		}
		break;
	}

	return pPaintbrush;
}

NS_VAYO2D_END
