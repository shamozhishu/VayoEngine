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
	ERT_NONE,
	ERT_WINDOW,
	ERT_MEMORY
};

class Body;
class Watcher;
class Graphics;
class Shape;
class Joint;
class Layer;
class LayerManager;
class Pivot;
class Renderer;
class Surface;
class SurfaceManager;
class Feature;
class FeatureManager;
class Geometry;
class Paintbrush;

typedef SharedPtr<Surface> SurfacePtr;
typedef SharedPtr<Feature> FeaturePtr;
typedef SharedPtr<Paintbrush> PaintbrushPtr;

NS_VAYO2D_END

#endif // __VAYO2D_SUPPORT_H__
