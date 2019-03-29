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
	ERT_MEMORY,
	ERT_WINDOW_DEFAULT,
	ERT_WINDOW_SOFTWARE,
	ERT_WINDOW_HARDWARE
};

class Body;
class Watcher;
class Graphics;
class Action;
class Shape;
class ShapeGroup;
class Surface;
class Joint;
class Layer;
class LayerManager;
class Pivot;
class Renderer;
class Picture;
class PictureManager;
class PicProcessor;
class Feature;
class FeatureManager;
class Paintbrush;
class Geometry;
class RectGeometry;
class RoundedRectGeometry;
class EllipseGeometry;
class PathGeometry;
class GeometryGroup;

typedef SharedPtr<Picture> PicturePtr;
typedef SharedPtr<Feature> FeaturePtr;
typedef SharedPtr<Paintbrush> PaintbrushPtr;

NS_VAYO2D_END

#endif // __VAYO2D_SUPPORT_H__
