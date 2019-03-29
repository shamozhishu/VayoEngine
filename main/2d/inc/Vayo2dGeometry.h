/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 几何形状
\*************************************************************************/
#ifndef __VAYO2D_GEOMETRY_H__
#define __VAYO2D_GEOMETRY_H__

#include "Vayo2dSupport.h"
#include "VayoVector2d.h"
#include "VayoRectangle.h"
#include "VayoMatrix3x3.h"
NS_VAYO2D_BEGIN

enum EGeometryType
{
	EGT_RECT,
	EGT_ROUNDED_RECT,
	EGT_ELLIPSE,
	EGT_PATH,
	EGT_GROUP
};

class Geometry
{
public:
	Geometry(const wstring& name, EGeometryType type)
		: _name(name), _type(type)
	{
		static unsigned long long idx = 0;
		if (0 == _name.compare(L""))
		{
			std::wstringstream ss;
			ss << L"Geometry" << idx << L"[" << _type << L"]";
			++idx;
			_name = ss.str();
		}
	}
	virtual ~Geometry() {}
	virtual bool isTransformed() const = 0;
	virtual bool transformed(const Matrix3x3& mat) = 0;
	template<typename T> T* geom_cast() const
	{
		switch (_type)
		{
		case EGT_RECT:
			if (typeid(T) == typeid(RectGeometry))
				return dynamic_cast<T*>((Geometry*)this);
			break;
		case EGT_ROUNDED_RECT:
			if (typeid(T) == typeid(RoundedRectGeometry))
				return dynamic_cast<T*>((Geometry*)this);
			break;
		case EGT_ELLIPSE:
			if (typeid(T) == typeid(EllipseGeometry))
				return dynamic_cast<T*>((Geometry*)this);
			break;
		case EGT_PATH:
			if (typeid(T) == typeid(PathGeometry))
				return dynamic_cast<T*>((Geometry*)this);
			break;
		case EGT_GROUP:
			if (typeid(T) == typeid(GeometryGroup))
				return dynamic_cast<T*>((Geometry*)this);
			break;
		default:
			return nullptr;
		}
		return nullptr;
	}

protected:
	PROPERTY_R_REF(wstring, _name, Name)
	PROPERTY_R(EGeometryType, _type, Type)
};

class RectGeometry : public Geometry
{
public:
	RectGeometry(const wstring& name) : Geometry(name, EGT_RECT) {}
	virtual bool buildRect(const Rectf& rect) = 0;
};

class RoundedRectGeometry : public Geometry
{
public:
	RoundedRectGeometry(const wstring& name) : Geometry(name, EGT_ROUNDED_RECT) {}
	virtual bool buildRoundedRect(const Rectf& rect, const Vector2df& radius) = 0;
};

class EllipseGeometry : public Geometry
{
public:
	EllipseGeometry(const wstring& name) : Geometry(name, EGT_ELLIPSE) {}
	virtual bool buildRound(const Vector2df& center, float radius) = 0;
	virtual bool buildEllipse(const Vector2df& center, const Vector2df& radius) = 0;
};

class PathGeometry : public Geometry
{
public:
	PathGeometry(const wstring& name) : Geometry(name, EGT_PATH) {}
	virtual bool beginFigure() = 0;
	virtual void endFigure() = 0;
	virtual void addLine(const Vector2df& pt) = 0;
	virtual void addLines(const Vector2df* pt, unsigned int size) = 0;
};

class GeometryGroup : public Geometry
{
public:
	GeometryGroup(const wstring& name) : Geometry(name, EGT_GROUP) {}
	virtual bool buildGroup() = 0;
	virtual void addGeometry(Geometry* pGeom) = 0;
	virtual void addGeometry(const wstring& geomName) = 0;
	virtual void removeGeometry(Geometry* pGeom) = 0;
	virtual void removeGeometry(const wstring& geomName) = 0;
	virtual void removeAllGeometries() = 0;
	virtual void setGeometries(const vector<Geometry*>& geoms) = 0;
};

NS_VAYO2D_END

#endif // __VAYO2D_GEOMETRY_H__
