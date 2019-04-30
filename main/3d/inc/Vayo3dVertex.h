/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 顶点类型
\*************************************************************************/
#ifndef __VAYO3D_VERTEX_H__
#define __VAYO3D_VERTEX_H__

#include "Vayo3dSupport.h"
#include "Vayo3dColour.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Vertex
{
public:
	Vertex() : _color(255, 255, 255, 255) {}
	Vertex(const Vector3df& p, const Vector3df& n, Colour clr, const Vector2df& uv)
		: _position(p), _normal(n), _color(clr), _texCoord(uv) {}
	Vertex(float px, float py, float pz, float nx, float ny, float nz, Colour clr, float u, float v)
		: _position(px, py, pz), _normal(nx, ny, nz), _color(clr), _texCoord(u, v) {}

	bool operator==(const Vertex& other) const
	{
		return (_position == other._position && _normal == other._normal &&
			_color == other._color && _texCoord == other._texCoord);
	}

	bool operator!=(const Vertex& other) const
	{
		return (_position != other._position || _normal != other._normal ||
			_color != other._color || _texCoord != other._texCoord);
	}

public:
	Vector3df  _position;
	Vector3df  _normal;
	Colour     _color;
	Vector2df  _texCoord;
};

struct VertIdxPair
{
	Vertex _vert;
	unsigned int _idx;
	VertIdxPair() : _idx(0) {}
	VertIdxPair(Vertex vertex) : _vert(vertex), _idx(0) {}
	VertIdxPair(Vertex vertex, unsigned int idx) : _vert(vertex), _idx(idx) {}
};

NS_VAYO3D_END

#endif // __VAYO3D_VERTEX_H__
