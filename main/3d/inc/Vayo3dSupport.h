/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 第三方支持
\*************************************************************************/
#ifndef __VAYO3D_SUPPORT_H__
#define __VAYO3D_SUPPORT_H__

#include "VayoSupport.h"

NS_VAYO3D_BEGIN

enum ERenderSystemType
{
	ERST_NULL,
	ERST_D3D9,
	ERST_D3D11,
	ERST_OPENGL
};

enum EPrimitiveType
{
	EPT_POINTS,
	EPT_LINES,
	EPT_LINE_LOOP,
	EPT_LINE_STRIP,
	EPT_TRIANGLES,
	EPT_TRIANGLE_STRIP,
	EPT_TRIANGLE_FAN,
	EPT_QUADS,
	EPT_QUAD_STRIP,
	EPT_POLYGON
};

enum EHardwareMapping
{
	EHM_NEVER = 0,
	EHM_STATIC,
	EHM_DYNAMIC,
	EHM_STREAM
};

enum EBufferType
{
	EBT_NONE = 0,
	EBT_VERTEX,
	EBT_INDEX,
	EBT_VERTEX_AND_INDEX
};

class Camera;
class ViewMemento;
class SpriteBox;
class MovableObject;
class ManualObject;
class Entity;
class Node;
class NodeAnimator;
class SceneNode;
class SceneLoader;
class SceneManager;
class TriContainer;
class SimpleTriContainer;
class CollisionDetector;
class Renderable;
class RenderSystem;
class UIManager;
class Image;
class Texture;
class TextureManager;
class Material;
class MaterialManager;
class MaterialRenderer;
class Vertex;
class VertexBuffer;
class IndexBuffer;
class SubMesh;
class SharedSubMesh;
class Mesh;
class MeshManager;
class Tesselator;
class DisplayList;
class MaterialRendererServices;
struct VertIdxPair;

typedef SharedPtr<Mesh> MeshPtr;
typedef SharedPtr<Texture> TexturePtr;
typedef SharedPtr<Material> MaterialPtr;
typedef SharedPtr<ViewMemento> ViewMementoPtr;
typedef SharedPtr<MaterialRenderer> MaterialRendererPtr;

template class _Vayo3dExport ReflexFactory<>;
template class _Vayo3dExport ReflexFactory<const wstring&, SceneManager*>;
template class _Vayo3dExport ReflexFactory<const wstring&, Node*, SceneManager*>;

NS_VAYO3D_END

#endif // __VAYO3D_SUPPORT_H__
