/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Direct2D位图
\*************************************************************************/
#ifndef __D2D_BITMAP_H__
#define __D2D_BITMAP_H__

#include "D2DSupport.h"
#include "Vayo2dPicture.h"

class D2DBitmap : public Picture
{
public:
	D2DBitmap(const wstring& name, D2DRenderer* renderer);
	~D2DBitmap();
	ComPtr<ID2D1Bitmap> getBitmap() const;

private:
	D2DRenderer* _renderer;
	ComPtr<ID2D1Bitmap> _bitmap;
};

#endif // __D2D_BITMAP_H__
