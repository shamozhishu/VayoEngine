#include "Vayo2dPictureManager.h"
#include "Vayo2dPicture.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "VayoSharedPtr.h"
#include "VayoLog.h"

NS_VAYO2D_BEGIN

PictureManager::PictureManager()
{
}

PictureManager::~PictureManager()
{
}

bool PictureManager::init()
{
	_picturePool.clear();
	return true;
}

PicturePtr PictureManager::getPicture(const wstring& filePath)
{
	if (filePath == L"")
		return NULL;

	map<wstring, PicturePtr>::iterator it = _picturePool.find(filePath);
	if (it != _picturePool.end())
		return it->second;

	PicturePtr picture = Pivot::getSingleton().getActiveRenderer()->createPicture(filePath);
	_picturePool.insert(make_pair(picture->getName(), picture));
	return picture;
}

void PictureManager::removePicture(const wstring& name)
{
	map<wstring, PicturePtr>::iterator it = _picturePool.find(name);
	if (it != _picturePool.end())
	{
		_picturePool.erase(it);
	}
}

void PictureManager::removePicture(PicturePtr picture)
{
	if (picture)
		removePicture(picture->getName());
}

NS_VAYO2D_END
