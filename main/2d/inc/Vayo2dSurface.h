/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 表面
\*************************************************************************/
#ifndef __VAYO2D_SURFACE_H__
#define __VAYO2D_SURFACE_H__

#include "Vayo2dSupport.h"
#include "Vayo2dBody.h"
#include "Vayo2dGraphics.h"
#include "VayoTable.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport Surface : public Body
{
public:
	class Section : public Graphics
	{
		friend class Surface;
		wstring _name;
	public:
		void render();
		PROPERTY_RW(Position2df, _pos, Pos)
	};

public:
	static Reflex<Surface, const wstring&, LayerManager*> _dynReflex;
	Surface(const wstring& name, LayerManager* oriLayerMgr);
	~Surface();
	void update(float dt);
	Section* getSection(const wstring& secName);
	void eraseSection(const wstring& secName);
	void eraseSection(Section* pSection);
	void eraseSections();

private:
	map<wstring, Section*> _sections;
};

NS_VAYO2D_END

#endif // __VAYO2D_SURFACE_H__
