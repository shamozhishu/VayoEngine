#include "Vayo2dPictureManager.h"
#include "Vayo2dPicture.h"
#include "VayoConfigManager.h"
#include "Vayo2dPivot.h"
#include "Vayo2dRenderer.h"
#include "VayoSharedPtr.h"
#include "VayoUtils.h"
#include "VayoLog.h"

NS_VAYO2D_BEGIN

PictureManager::PictureManager()
	: _picProcessor(nullptr)
{
}

PictureManager::~PictureManager()
{
}

bool PictureManager::init()
{
	_picturePool.clear();
	_picProcessor = Pivot::getSingleton().getActiveRenderer()->getPicProcessor();
	return _picProcessor != nullptr;
}

PicProcessor& PictureManager::getPicProcessor()
{
	return *_picProcessor;
}

PicturePtr PictureManager::getPicture(const wstring& filename, bool fullPath /*= false*/)
{
	if (filename == L"")
		return nullptr;

	wstring fileName = filename;
	trim(fileName);
	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._2d.picturesPath + fileName;

	map<wstring, PicturePtr>::iterator it = _picturePool.find(filePath);
	if (it != _picturePool.end())
		return it->second;

	PicturePtr picture = Pivot::getSingleton().getActiveRenderer()->createPicture(filePath);
	_picturePool.insert(make_pair(picture->getFileName(), picture));
	return picture;
}

void PictureManager::removePicture(const wstring& filename, bool fullPath /*= false*/)
{
	wstring fileName = filename;
	trim(fileName);
	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._2d.picturesPath + fileName;

	map<wstring, PicturePtr>::iterator it = _picturePool.find(filePath);
	if (it != _picturePool.end())
		_picturePool.erase(it);
}

void PictureManager::removePicture(PicturePtr picture)
{
	if (picture)
		removePicture(picture->getFileName(), true);
}

void PictureManager::cleanAllPictures()
{
	map<wstring, PicturePtr>::iterator it = _picturePool.begin();
	for (; it != _picturePool.end(); ++it)
		it->second->cleanPic();
}

Dimension2di PictureManager::getPicOptimizedSize(Dimension2di size)
{
	Dimension2di picOptimizeSize = Dimension2di(0x01, 0x01);
	while (picOptimizeSize._width < size._width)
		picOptimizeSize._width <<= 1;
	while (picOptimizeSize._height < size._height)
		picOptimizeSize._height <<= 1;
	return picOptimizeSize;
}

NS_VAYO2D_END
