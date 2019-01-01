/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 纹理管理器
\*************************************************************************/
#ifndef __VAYO_TEXTURE_MANAGER_H__
#define __VAYO_TEXTURE_MANAGER_H__

#include "VayoSupport.h"
#include "VayoTexture.h"
NS_VAYO_BEGIN

class _VayoExport TextureManager
{
	DISALLOW_COPY_AND_ASSIGN(TextureManager)
public:
	TextureManager();
	~TextureManager();
	bool       init();
	TexturePtr getTexture(const wstring& filePath);
	void       removeTexture(const wstring& name);
	void       removeTexture(TexturePtr tex);

private:
	map<wstring, TexturePtr> _texturePool;
};

NS_VAYO_END

#endif // __VAYO_TEXTURE_MANAGER_H__
