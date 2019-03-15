/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 图片处理器
\*************************************************************************/
#ifndef __VAYO2D_PIC_PROCESSOR_H__
#define __VAYO2D_PIC_PROCESSOR_H__

#include "Vayo2dSupport.h"
#include "Vayo2dColor.h"
#include "VayoRectangle.h"
NS_VAYO2D_BEGIN

class PicProcessor
{
public:
	virtual ~PicProcessor() {}
	virtual bool load(PicturePtr picDst, unsigned int width = 0, unsigned int height = 0) = 0;
	virtual bool loadByMmap(PicturePtr picDst, unsigned int width = 0, unsigned int height = 0) = 0;
	virtual bool save(PicturePtr picDst, const PicturePtr picSrc) = 0;
	virtual bool save(PicturePtr picDst, const PicturePtr picSrc, const Rectf& dstRect) = 0;
	virtual bool blend(PicturePtr picDst, const PicturePtr picSrc, Color clr) = 0;
	virtual bool blend(PicturePtr picDst, const PicturePtr picSrc1, const PicturePtr picSrc2, float proportion) = 0;
};

NS_VAYO2D_END

#endif // __VAYO2D_PIC_PROCESSOR_H__
