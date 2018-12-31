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
	: m_plane(_T("XYÆ½Ãæ"))
{
}

CGridPlace::~CGridPlace()
{
}
//------------------------------------------------------------------------
CGridShapeOp::CGridShapeOp()
	: m_hasTopCaps(false)
	, m_hasBottomCaps(false)
	, m_hasStretchBodies(false)
{
	m_topCap.reverse = false;
	m_topCap.normal.set(0, 0, 1);

	m_bottomCap.reverse = true;
	m_bottomCap.normal.set(0, 0, -1);

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
	, m_needUpdateModel(true)
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

bool CGridData::operator==(const CGridData &rhs) const
{
	return m_prop.m_modelName == rhs.m_prop.m_modelName;
}
