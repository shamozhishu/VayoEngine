#pragma once

class CGridProp
{
	friend class CGridDataManager;
public:
	CGridProp();
	~CGridProp();

private:
	CString m_modelName;
	CString m_materialName;
};

class CGridPlace
{
	friend class CGridDataManager;
public:
	CGridPlace();
	~CGridPlace();

private:
	CString m_place;
	SpatialInfo m_spaceInfo;
};

class CGridShapeOp
{
	friend class CGridDataManager;
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
		vector<SpatialInfo> spaceInfo;
	};

private:
	SCap m_topCaps;
	SCap m_bottomCaps;
	SStretchingBody m_stretchingBodies;
};

class CGridCircle
{
	friend class CGridDataManager;
public:
	CGridCircle();
	~CGridCircle();

private:
	float m_diameter;
	Vector2df m_pos;
	unsigned int m_cnt;
	bool m_clockwise;
	CGridShapeOp m_shapeOp;
};

class CGridPolygon
{
	friend class CGridDataManager;
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

private:
	vector<SPolyPoint> m_polyPts;
	CGridShapeOp m_shapeOp;
};

class CGridData
{
	friend class CGridDataManager;
public:
	CGridData();
	~CGridData();
	void ClearAll();

private:
	CGridProp m_prop;
	CGridPlace m_place;
	bool m_isTopCapTessAll;
	bool m_isBottomCapTessAll;
	CGridShapeOp::SCap m_topCap;
	CGridShapeOp::SCap m_bottomCap;
	unsigned int m_circBaseContourIdx;
	unsigned int m_polyBaseContourIdx;
	vector<CGridCircle> m_circles;
	vector<CGridPolygon> m_polygons;
};
