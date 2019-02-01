/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 纹理管理器
\*************************************************************************/
#ifndef __VAYO3D_TEXTURE_MANAGER_H__
#define __VAYO3D_TEXTURE_MANAGER_H__

#include "Vayo3dSupport.h"
#include "Vayo3dTexture.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport TextureManager
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

NS_VAYO3D_END

#endif // __VAYO3D_TEXTURE_MANAGER_H__
