/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* »­Ë¢
\*************************************************************************/
#ifndef __VAYO2D_PAINTBRUSH_H__
#define __VAYO2D_PAINTBRUSH_H__

#include "Vayo2dSupport.h"
#include "Vayo2dFeature.h"
NS_VAYO2D_BEGIN

class Paintbrush
{
public:
	virtual ~Paintbrush() {}
	virtual void onSetFeature(Feature& feature, const Feature& lastFeature) {}
	virtual void onUnsetFeature() {}
	virtual bool isTransparent() const { return false; }
};

NS_VAYO2D_END

#endif // __VAYO2D_PAINTBRUSH_H__
