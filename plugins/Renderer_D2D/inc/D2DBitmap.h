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
	friend class D2DPicProcessor;
public:
	D2DBitmap(const wstring& filename, D2DRenderer* renderer);
	void destroy();
	void cleanPic();
	bool isLoaded() const;
	Dimension2df getPicSize() const;
	ComPtr<ID2D1Bitmap> getBitmap(EDeviceID devid = EDID_CURRENT_DEV);

private:
	D2DRenderer* _renderer;
	ComPtr<IWICBitmap> _wicBitmap;
	ComPtr<IWICFormatConverter> _wicConverter;
	ComPtr<ID2D1Bitmap> _bitmap[EDID_DEVICE_COUNT];
};

#endif // __D2D_BITMAP_H__
