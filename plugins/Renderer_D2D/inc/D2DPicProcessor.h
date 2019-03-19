/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* Direct2DͼƬ������
\*************************************************************************/
#ifndef __D2D_PIC_PROCESSOR_H__
#define __D2D_PIC_PROCESSOR_H__

#include "D2DSupport.h"
#include "Vayo2dPicProcessor.h"

class D2DPicProcessor : public PicProcessor
{
public:
	D2DPicProcessor(D2DRenderer* renderer);
	bool load(PicturePtr picDst, unsigned int width = 0, unsigned int height = 0);
	bool loadByMmap(PicturePtr picDst, unsigned int width = 0, unsigned int height = 0);
	bool save(PicturePtr picDst, const PicturePtr picSrc);
	bool save(PicturePtr picDst, const PicturePtr picSrc, const Rectf& dstRect);
	bool blend(PicturePtr picDst, const PicturePtr picSrc, Color clr);
	bool blend(PicturePtr picDst, const PicturePtr picBelow, const PicturePtr picAbove, float proportion);

private:
	D2DRenderer* _renderer;
};

#endif // __D2D_PIC_PROCESSOR_H__
