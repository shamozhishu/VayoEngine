#pragma once

class CGridProp
{
public:
	CGridProp();
	~CGridProp();

	CString m_modelName;
	CString m_materialName;
};

class CGridPlace
{
public:
	CGridPlace();
	~CGridPlace();

	CString m_plane;
	SpatialInfo m_spaceInfo;
};

class CGridShapeOp
{
public:
	CGridShapeOp();
	~CGridShapeOp();

	struct SCap
	{
		bool reverse;
		CString materialName;
		Vector3df normal;
	};

	struct SStretchingBody
	{
		bool reverse;
		CString materialName;
		list<SpatialInfo> spaceInfo;
	};

	bool m_hasTopCaps;
	bool m_hasBottomCaps;
	bool m_hasStretchBodies;
	SCap m_topCap;
	SCap m_bottomCap;
	SStretchingBody m_stretchingBodies;
};

class CGridCircle
{
public:
	CGridCircle();
	~CGridCircle();

	float m_diameter;
	Vector2df m_pos;
	unsigned int m_cnt;
	bool m_clockwise;
	CGridShapeOp m_shapeOp;
};

class CGridPolygon
{
public:
	CGridPolygon();
	~CGridPolygon();

	struct SPolyPoint
	{
		unsigned int m_idx;
		Vector2df m_pos;
		float m_degree;
		unsigned int m_cnt;
		bool m_clockwise;
	};

	list<SPolyPoint> m_polyPts;
	CGridShapeOp m_shapeOp;
};

class CGridData
{
public:
	CGridData();
	~CGridData();
	void ClearAll();
	bool operator==(const CGridData &rhs) const;

	CGridProp m_prop;
	CGridPlace m_place;
	bool m_needUpdateModel;
	bool m_isTopCapTessAll;
	bool m_isBottomCapTessAll;
	CGridShapeOp::SCap m_topCap;
	CGridShapeOp::SCap m_bottomCap;
	unsigned int m_circBaseContourIdx;
	unsigned int m_polyBaseContourIdx;
	list<CGridCircle> m_circles;
	list<CGridPolygon> m_polygons;
};
