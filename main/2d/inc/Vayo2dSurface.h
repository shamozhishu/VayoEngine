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
NS_VAYO2D_BEGIN

class Surface : public Body
{
public:
	class Block
	{
		friend class Surface;
		Block();
		~Block();
	};

	class Section : public Graphics
	{
		friend class Surface;
	public:
		void render();
	private:
		Section();
		~Section();
		wstring _name;
	private:
		PROPERTY_RW(Position2df, _pos, Pos)
	};

public:
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
