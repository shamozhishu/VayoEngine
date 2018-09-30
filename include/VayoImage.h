/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 图像解析类
\*************************************************************************/
#ifndef __VAYO_IMAGE_H__
#define __VAYO_IMAGE_H__

#include "VayoSupport.h"
#include "VayoColour.h"
#include "VayoDimension2d.h"
NS_VAYO_BEGIN

class _VayoExport Image
{
public:
	Image();
	Image(EColorFormat format, const Dimension2di& size);
	Image(const Image& img);
	~Image();
	Image&              operator=(const Image& img);
	bool                load(const wstring& filename);
	void                save(const wstring& filename);
	void*               lock();
	void                unlock();
	void                reset(EColorFormat format, const Dimension2di& size);
	const Dimension2di& getDimension() const;
	int                 getBitsPerPixel() const;
	int                 getBytesPerPixel() const;
	int                 getImageDataSizeInBytes() const;
	int                 getImageDataSizeInPixels() const;
	Colour              getPixel(unsigned int x, unsigned int y) const;
	void                setPixel(unsigned int x, unsigned int y, const Colour& color);
	EColorFormat        getColorFormat() const;
	int                 getPitch() const;

private:
	void freeMemory();
private:
	unsigned char* _data;
	Dimension2di   _size;
	int            _bytesPerPixel;
	int            _pitch;
	EColorFormat   _format;
};

NS_VAYO_END

#endif // __VAYO_IMAGE_H__
