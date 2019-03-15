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
#include "VayoFrameListener.h"
NS_VAYO2D_BEGIN

class Surface : public Body, public FrameListener
{
public:
	class Block;
	class Section : public Graphics
	{
		~Section();
		wstring _name;
		Table<Block> _blockSet;
		friend class Surface;
		PROPERTY_RW(Position2df, _pos, Pos)
	public:
		void render();
	};

	class Block
	{
		PicturePtr _pic;
		friend class Surface;
		friend class Surface::Section;
	public:
		~Block();
	};

public:
	Surface(const wstring& name, LayerManager* oriLayerMgr);
	~Surface();
	void update(float dt);
	void blockSplit(int rows, int cols, Rectf scope, Dimension2di maxSize);
	bool frameStarted(float dt);
	bool frameRendering(float dt);
	Section* getSection(const wstring& secName);
	void eraseSection(const wstring& secName);
	void eraseSection(Section* pSection);
	void eraseSections();

private:
	bool                 _needUpdate;
	int                   _blockRows;
	int                   _blockCols;
	Rectf                     _scope;
	Dimension2di            _maxSize;
	map<wstring, Section*> _sections;
};

NS_VAYO2D_END

#endif // __VAYO2D_SURFACE_H__
