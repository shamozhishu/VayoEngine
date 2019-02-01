/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Direct2DäÖÈ¾Æ÷
\*************************************************************************/
#ifndef __D2D_RENDERER_H__
#define __D2D_RENDERER_H__

#include "D2DSupport.h"
#include "Vayo2dRenderer.h"

class D2DRenderer : public Renderer
{
public:
	D2DRenderer();
	~D2DRenderer();

	bool beginDraw();
	bool endDraw();
	void drawRect(const Rectf& rc);
	void drawRect(float x, float y, float w, float h);
	void drawRound(float radius, float center);
	void drawGeometry();

	bool setRenderTarget(ERenderTarget rt);
};

#endif // __D2D_RENDERER_H__
