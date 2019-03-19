/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Í¼Æ¬
\*************************************************************************/
#ifndef __VAYO2D_PICTURE_H__
#define __VAYO2D_PICTURE_H__

#include "Vayo2dSupport.h"
#include "VayoDimension2d.h"
NS_VAYO2D_BEGIN

class Picture
{
public:
	Picture(const wstring& filename) : _filename(filename) {}
	virtual ~Picture() {}
	virtual void destroy() = 0;
	virtual void cleanPic() = 0;
	virtual bool isLoaded() const = 0;
	virtual Dimension2df getPicSize() const = 0;
protected:
	PROPERTY_R_REF(wstring, _filename, FileName)
};

NS_VAYO2D_END

#endif // __VAYO2D_PICTURE_H__
