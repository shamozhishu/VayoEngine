/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 纹理贴图
\*************************************************************************/
#ifndef __VAYO_TEXTURE_H__
#define __VAYO_TEXTURE_H__

#include "VayoSupport.h"
#include "VayoColour.h"
#include "VayoRectangle.h"
NS_VAYO_BEGIN

class Texture
{
public:
	Texture(const wstring& name) : _name(name) {}
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
	PROPERTY_R_REF(wstring, _name, Name)
};

NS_VAYO_END

#endif // __VAYO_TEXTURE_H__
