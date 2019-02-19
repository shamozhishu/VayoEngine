#include "Vayo2dSurfaceManager.h"
#include "Vayo2dSurface.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "VayoSharedPtr.h"
#include "VayoLog.h"

NS_VAYO2D_BEGIN

SurfaceManager::SurfaceManager()
{
}

SurfaceManager::~SurfaceManager()
{
}

bool SurfaceManager::init()
{
	_surfacePool.clear();
	return true;
}

SurfacePtr SurfaceManager::getSurface(const wstring& filePath)
{
	if (filePath == L"")
		return NULL;

	map<wstring, SurfacePtr>::iterator it = _surfacePool.find(filePath);
	if (it != _surfacePool.end())
		return it->second;

	SurfacePtr surface = Pivot::getSingleton().getActiveRenderer()->createSurface(filePath);
	if (!surface)
	{
		Log::wprint(ELL_ERROR, L"Could not create Direct2D surface: %s", filePath.c_str());
		return NULL;
	}

	_surfacePool.insert(make_pair(surface->getName(), surface));
	return surface;
}

void SurfaceManager::removeSurface(const wstring& name)
{
	map<wstring, SurfacePtr>::iterator it = _surfacePool.find(name);
	if (it != _surfacePool.end())
	{
		_surfacePool.erase(it);
	}
}

void SurfaceManager::removeSurface(SurfacePtr surface)
{
	if (surface)
		removeSurface(surface->getName());
}

NS_VAYO2D_END
