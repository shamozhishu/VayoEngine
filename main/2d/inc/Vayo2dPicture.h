/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Í¼Æ¬
\*************************************************************************/
#ifndef __VAYO2D_PICTURE_H__
#define __VAYO2D_PICTURE_H__

#include "Vayo2dSupport.h"
#include "math/VayoDimension2d.h"
NS_VAYO2D_BEGIN

class Picture
{
public:
	Picture(const wstring& name) : _name(name) {}
	virtual ~Picture() {}

protected:
	PROPERTY_R_REF(wstring, _name, Name)
	PROPERTY_RW_REF(Dimension2df, _size, Size)
};

NS_VAYO2D_END

#endif // __VAYO2D_PICTURE_H__
