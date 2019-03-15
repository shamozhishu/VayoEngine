#include "Vayo3dTextureManager.h"
#include "VayoConfigManager.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"
#include "Vayo3dImage.h"
#include "VayoUtils.h"

NS_VAYO3D_BEGIN

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

bool TextureManager::init()
{
	_texturePool.clear();
	return true;
}

TexturePtr TextureManager::getTexture(const wstring& filename, bool fullPath /*= false*/)
{
	if (filename == L"")
		return NULL;

	wstring fileName = filename;
	trim(fileName);
	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._3d.texturesPath + fileName;

	map<wstring, TexturePtr>::iterator it = _texturePool.find(filePath);
	if (it != _texturePool.end())
		return it->second;

	Image image;
	if (!image.load(filePath))
	{
		Log::wprint(ELL_ERROR, L"Could not load texture: %s", filePath.c_str());
		return NULL;
	}

	TexturePtr pTex = Root::getSingleton().getActiveRenderer()->createTexture(filePath, &image, true);
	_texturePool.insert(make_pair(pTex->getFileName(), pTex));
	return pTex;
}

void TextureManager::removeTexture(const wstring& filename, bool fullPath /*= false*/)
{
	wstring fileName = filename;
	trim(fileName);
	wstring filePath;
	if (fullPath)
		filePath = fileName;
	else
		filePath = ConfigManager::getSingleton().getConfig()._3d.texturesPath + fileName;

	map<wstring, TexturePtr>::iterator it = _texturePool.find(filePath);
	if (it != _texturePool.end())
		_texturePool.erase(it);
}

void TextureManager::removeTexture(TexturePtr tex)
{
	if (tex)
		removeTexture(tex->getFileName(), true);
}

NS_VAYO3D_END
