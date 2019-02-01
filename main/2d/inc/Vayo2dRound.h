/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Ô²
\*************************************************************************/
#ifndef __VAYO2D_ROUND_H__
#define __VAYO2D_ROUND_H__

#include "Vayo2dShape.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Round : public Shape
{
public:
	Round(float x, float y, float diameter, unsigned int color);
	~Round();
	void render();

protected:
};

NS_VAYO2D_END

#endif // __VAYO2D_ROUND_H__
