/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* äÖÈ¾Æ÷
\*************************************************************************/
#ifndef __VAYO2D_RENDERER_H__
#define __VAYO2D_RENDERER_H__

#include "Vayo2dSupport.h"
#include "math/VayoRectangle.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Renderer
{
public:
	Renderer();
	virtual ~Renderer();

	virtual bool beginDraw() = 0;
	virtual bool endDraw() = 0;
	virtual void drawRect(const Rectf& rc) = 0;
	virtual void drawRect(float x, float y, float w, float h) = 0;
	virtual void drawRound(float radius, float center) = 0;
	virtual void drawGeometry() = 0;

	virtual bool setRenderTarget(ERenderTarget rt);

private:
	ERenderTarget _renderTarget;
};

NS_VAYO2D_END

#endif // __VAYO2D_RENDERER_H__
