/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* Direct2D位图
\*************************************************************************/
#ifndef __D2D_SURFACE_H__
#define __D2D_SURFACE_H__

#include "D2DSupport.h"
#include "Vayo2dSurface.h"

class D2DSurface : public Surface
{
public:
	D2DSurface(const wstring& name, D2DRenderer* renderer);
	~D2DSurface();

private:
	D2DRenderer* _renderer;
};

#endif // __D2D_SURFACE_H__
