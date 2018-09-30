#include "VayoTextureManager.h"
#include "VayoConfigManager.h"
#include "VayoRenderSystem.h"
#include "VayoRoot.h"
#include "VayoLog.h"
#include "VayoImage.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
}

bool TextureManager::init()
{
	return true;
}

TexturePtr TextureManager::getTexture(const wstring& filePath)
{
	if (filePath == L"")
		return NULL;

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
	if (!pTex)
	{
		Log::wprint(ELL_ERROR, L"Could not create OpenGL texture: %s", filePath.c_str());
		return NULL;
	}

	_texturePool.insert(make_pair(pTex->getName(), pTex));
	return pTex;
}

void TextureManager::removeTexture(const wstring& name)
{
	map<wstring, TexturePtr>::iterator it = _texturePool.find(name);
	if (it != _texturePool.end())
	{
		_texturePool.erase(it);
	}
}

void TextureManager::removeTexture(TexturePtr tex)
{
	if (tex)
		removeTexture(tex->getName());
}

NS_VAYO_END
