/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 2D支持
\*************************************************************************/
#ifndef __VAYO2D_SUPPORT_H__
#define __VAYO2D_SUPPORT_H__

#include "VayoSupport.h"
using namespace Vayo;

NS_VAYO2D_BEGIN

enum ERenderTarget
{
	ERT_HWND,
	ERT_BITMAP
};

class Body;
class Shape;
class Joint;
class Layer;
class LayerManager;
class Pivot;
class Renderer;
class Surface;
class Feature;
class FeatureManager;

typedef SharedPtr<Surface> SurfacePtr;
typedef SharedPtr<Feature> FeaturePtr;

NS_VAYO2D_END

#endif // __VAYO2D_SUPPORT_H__
