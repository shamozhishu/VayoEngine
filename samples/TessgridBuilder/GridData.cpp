#include "stdafx.h"
#include "GridData.h"

CGridModel::CGridModel()
{
}

CGridModel::~CGridModel()
{
}
//------------------------------------------------------------------------
CGridPlace::CGridPlace()
	: m_plane(_T("XY平面"))
{
}

CGridPlace::~CGridPlace()
{
}
//------------------------------------------------------------------------
CGridShapeOp::CGridShapeOp()
	: m_hasTopCaps(false)
	, m_hasBottomCaps(false)
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
	: m_needUpdateModel(true)
	, m_isTopCapTessAll(false)
	, m_isBottomCapTessAll(false)
	, m_circBaseContourIdx(0)
	, m_polyBaseContourIdx(0)
{
	m_topCap.reverse = false;
	m_topCap.normal.set(0, 0, 1);

	m_bottomCap.reverse = true;
	m_bottomCap.normal.set(0, 0, -1);
}

CGridData::~CGridData()
{
}

void CGridData::ClearAll()
{
	m_needUpdateModel = true;
	m_isTopCapTessAll = false;
	m_isBottomCapTessAll = false;
	m_circBaseContourIdx = 0;
	m_polyBaseContourIdx = 0;
	m_circles.clear();
	m_polygons.clear();

	m_model.m_modelName.Empty();

	m_place.m_plane = _T("XY平面");
	m_place.m_spaceInfo = SpatialInfo();

	m_topCap.materialName.Empty();
	m_topCap.reverse = false;
	m_topCap.normal.set(0, 0, 1);

	m_bottomCap.materialName.Empty();
	m_bottomCap.reverse = true;
	m_bottomCap.normal.set(0, 0, -1);
}

bool CGridData::operator==(const CGridData &rhs) const
{
	return m_model.m_modelName == rhs.m_model.m_modelName;
}
