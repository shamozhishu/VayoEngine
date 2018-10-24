/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 第三方支持
\*************************************************************************/
#ifndef __VAYO_SUPPORT_H__
#define __VAYO_SUPPORT_H__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <typeinfo>
#include <memory>
// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
}

#include "VayoCommon.h"
using namespace std;

namespace Vayo
{
	class App;
	class Camera;
	class SpriteBox;
	class MovableObject;
	class Node;
	class NodeAnimator;
	class SceneNode;
	class SceneLoader;
	class SceneManager;
	class TriContainer;
	class SimpleTriContainer;
	class CollisionDetector;
	class KeypadDispatcher;
	class TouchDispatcher;
	class Device;
	class Renderable;
	class RenderSystem;
	class ScriptSystem;
	class ConfigManager;
	class DatabaseCSV;
	class Language;
	class UIManager;
	class Image;
	class Texture;
	class TextureManager;
	class Material;
	class MaterialManager;
	class MaterialRenderer;
	class Vertex;
	class Mesh;
	class SubMesh;
	class MeshManager;
	class Tesselator;
	class DisplayList;
	class Plugin;
	class DynLib;
	class DynLibManager;
	class MaterialRendererServices;

	template<typename T> class SharedPtr;
	typedef SharedPtr<Mesh>             MeshPtr;
	typedef SharedPtr<Texture>          TexturePtr;
	typedef SharedPtr<Material>         MaterialPtr;
	typedef SharedPtr<MaterialRenderer> MaterialRendererPtr;
}

#endif // __VAYO_SUPPORT_H__
