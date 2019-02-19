/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Direct2D»­Ë¢
\*************************************************************************/
#ifndef __D2D_PAINTBRUSH_H__
#define __D2D_PAINTBRUSH_H__

#include "D2DSupport.h"
#include "Vayo2dPaintbrush.h"

class D2DPaintbrush : public Paintbrush
{
	friend class D2DRenderer;
public:
	D2DPaintbrush(ERenderTarget rt, unsigned int devid, D2DRenderer* renderer);
	~D2DPaintbrush();
	void onSetFeature(Feature& feature, const Feature& lastFeature);
	void onUnsetFeature();

private:
	D2DRenderer*          _renderer;
	ID2D1SolidColorBrush* _colorBrush;
	IDWriteTextFormat*    _textFormat;
	ID2D1StrokeStyle*     _strokeStyle;
};

#endif // __D2D_PAINTBRUSH_H__
