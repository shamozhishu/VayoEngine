#include "stdafx.h"
#include "GridData.h"

CGridProp::CGridProp()
{
}

CGridProp::~CGridProp()
{
}
//------------------------------------------------------------------------
CGridPlace::CGridPlace()
	: m_place(_T("XYÆ½Ãæ"))
{
}

CGridPlace::~CGridPlace()
{
}
//------------------------------------------------------------------------
CGridShapeOp::CGridShapeOp()
{
	m_topCaps.reverse = false;
	m_topCaps.normal.set(0, 0, 1);

	m_bottomCaps.reverse = true;
	m_bottomCaps.normal.set(0, 0, -1);

	m_stretchingBodies.reverse = false;
}

CGridShapeOp::~CGridShapeOp()
{
}
//------------------------------------------------------------------------
CGridCircle::CGridCircle()
	: m_diameter(1.0f)
	, m_cnt(60)
	, m_clockwise(false)
{
}

CGridCircle::~CGridCircle()
{
}
//------------------------------------------------------------------------
CGridPolygon::CGridPolygon()
{
}

CGridPolygon::~CGridPolygon()
{
}
//------------------------------------------------------------------------
CGridData::CGridData()
	: m_isTopCapTessAll(false)
	, m_isBottomCapTessAll(false)
	, m_circBaseContourIdx(0)
	, m_polyBaseContourIdx(0)
{
}

CGridData::~CGridData()
{
}

void CGridData::ClearAll()
{
	m_isTopCapTessAll = false;
	m_isBottomCapTessAll = false;
	m_circBaseContourIdx = 0;
	m_polyBaseContourIdx = 0;
	m_circles.clear();
	m_polygons.clear();
}
