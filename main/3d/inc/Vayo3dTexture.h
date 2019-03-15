/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 纹理贴图
\*************************************************************************/
#ifndef __VAYO3D_TEXTURE_H__
#define __VAYO3D_TEXTURE_H__

#include "Vayo3dSupport.h"
#include "Vayo3dColour.h"
#include "VayoRectangle.h"
NS_VAYO3D_BEGIN

class Texture
{
public:
	Texture(const wstring& filename) : _filename(filename) {}
	virtual ~Texture() {}
	virtual void*               lock(EColorFormat fmt, Recti rc) = 0;
	virtual void                unlock() = 0;
	virtual const Dimension2di& getImgSize() const = 0;
	virtual const Dimension2di& getTexSize() const = 0;
	virtual EColorFormat        getColorFormat() const = 0;
	virtual ERenderSystemType   getRenderSystemType() const = 0;
	virtual int                 getPitch() const = 0;
	virtual bool                hasMipMaps() const { return false; }

protected:
	PROPERTY_R_REF(wstring, _filename, FileName)
};

NS_VAYO3D_END

#endif // __VAYO3D_TEXTURE_H__
