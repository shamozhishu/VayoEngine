#include "Vayo2dRenderer.h"

NS_VAYO2D_BEGIN

bool Renderer::setRenderTarget(ERenderTarget rt)
{
	_renderTarget = rt;
	return true;
}

Renderer::Renderer(const wstring& name)
	: _name(name)
	, _renderTarget(ERT_WINDOW)
{
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

Paintbrush* Renderer::getPaintbrush(unsigned int devid)
{
	if (devid >= EDID_DEVICE_COUNT)
		return nullptr;

	Paintbrush* pPaintbrush = nullptr;
	switch (_renderTarget)
	{
	case ERT_NONE:
		return nullptr;
	case ERT_WINDOW:
		pPaintbrush = _hwndPaintbrushs[devid].get();
		if (!pPaintbrush)
		{
			_hwndPaintbrushs[devid] = createPaintbrush(ERT_WINDOW, devid);
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
