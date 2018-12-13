#include "stdafx.h"
#include "GridDataManager.h"

static CGridDataManager* g_pDataMedelMgr = NULL;

CGridDataManager& CGridDataManager::GetIns()
{
	if (!g_pDataMedelMgr)
		g_pDataMedelMgr = new CGridDataManager();
	return *g_pDataMedelMgr;
}

CGridDataManager::CGridDataManager()
{
}

CGridDataManager::~CGridDataManager()
{
}

bool CGridDataManager::OpenTessgridFile(CString filePath)
{
	if (!filePath.IsEmpty() && filePath.Find(_T(".tessgrid")) != -1)
	{
		std::wifstream fin(filePath.GetString());
		if (!fin)
		{
			Log::wprint(ELL_ERROR, L"文件[%s]打开失败", filePath.GetString());
			return false;
		}

		fin.imbue(locale("chs"));
		wstringstream filestream;
		filestream << fin.rdbuf();

		if (ParseTessgridFile(filestream))
		{
			Log::wprint(ELL_INFORMATION, L"文件[%s]解析成功", filePath.GetString());
			return true;
		}

		Log::wprint(ELL_ERROR, L"文件[%s]解析失败", filePath.GetString());
	}

	return false;
}

bool CGridDataManager::ParseTessgridFile(wstringstream& filestream)
{
	m_gridDataset.clear();
	bool succParse = false;
	wstring tag;

	while (!filestream.eof())
	{
		filestream >> tag;
		if (tag == L"")
			continue;

		if (tag == L"#gridding")
		{
			tag.clear();
			succParse = ParseGridding(filestream);
			IF_FALSE_BREAK(succParse);
			continue;
		}

		if (tag != L"#gridding")
		{
			succParse = false;
			break;
		}
	}

	if (!succParse)
		Log::wprint(ELL_WARNING, L"文件流解析失败");

	return succParse;
}

bool CGridDataManager::ParseProp(wstringstream& strin)
{
	wstring tag;
	strin >> tag;
	if (tag != L"@prop")
		return false;

	strin >> tag;
	m_currentGridData.m_prop.m_modelName = tag.c_str();

	strin >> tag;
	if (tag != L"default_material")
		m_currentGridData.m_prop.m_materialName = tag.c_str();

	unsigned int ignore;
	strin >> ignore >> ignore;

	strin >> tag;
	if (tag != L"@end_prop")
		return false;

	return true;
}

bool CGridDataManager::ParseCapAndBody(wstringstream& strin, const wstring& tag)
{
	wstring aTag = tag;

	if (aTag == L"@top_cap")
	{
		if (!ParseCap(strin, true))
			return false;
		strin >> aTag;
	}

	if (aTag == L"@stretching_body")
	{
		if (!ParseStretchingBody(strin))
			return false;
		strin >> aTag;
	}

	if (aTag == L"@bottom_cap")
	{
		if (!ParseCap(strin, false))
			return false;
		strin >> aTag;
	}

	if (aTag != L"#end_gridding")
		return false;

	m_gridDataset.push_back(m_currentGridData);
	m_currentGridData.ClearAll();
	return true;
}

bool CGridDataManager::ParseCap(wstringstream& strin, bool topCap)
{
	CGridShapeOp::SCap capData;
	CGridShapeOp::SStretchingBody bodyData;

	int cnt;
	Vector3df norm;
	bool reverse;
	wstring materialname, tag;
	strin >> cnt;
	for (int i = 0; i < cnt; ++i)
	{
		strin >> reverse >> materialname >> norm._x >> norm._y >> norm._z >> tag;

		if (tag == L"index")
		{
			int idx;
			strin >> idx;
			if (!OperateCurGridData(idx, (topCap ? EGRIDDATA_OP_TOPCAP : EGRIDDATA_OP_BOTTOMCAP), capData, bodyData))
			{
				return false;
			}
		}
		else if (tag == L"range")
		{
			int contourLowerIdx, contourUpperIdx;
			strin >> contourLowerIdx >> contourUpperIdx;
			for (int idx = contourLowerIdx; idx <= contourUpperIdx; ++idx)
			{
				if (!OperateCurGridData(idx, (topCap ? EGRIDDATA_OP_TOPCAP : EGRIDDATA_OP_BOTTOMCAP), capData, bodyData))
				{
					return false;
				}
			}
		}
		else if (tag == L"array")
		{
			int idxNum, idx;
			strin >> idxNum;

			for (int j = 0; j < idxNum; ++j)
			{
				strin >> idx;
				if (!OperateCurGridData(idx, (topCap ? EGRIDDATA_OP_TOPCAP : EGRIDDATA_OP_BOTTOMCAP), capData, bodyData))
				{
					return false;
				}
			}
		}
		else
			return false;
	}

	strin >> tag;
	if (tag != L"@end_cap")
		return false;
	return true;
}

bool CGridDataManager::ParsePlace(wstringstream& strin)
{
	wstring tag;
	strin >> tag;
	if (tag == L"xy")
		m_currentGridData.m_place.m_place = _T("XY平面");
	else if (tag == L"xz")
		m_currentGridData.m_place.m_place = _T("XZ平面");
	else if (tag == L"yz")
		m_currentGridData.m_place.m_place = _T("YZ平面");
	else
		return false;

	Vector3df offset;
	Vector3df rot;
	Vector3df scale;
	strin >> offset._x >> offset._y >> offset._z;
	strin >> rot._x >> rot._y >> rot._z;
	strin >> scale._x >> scale._y >> scale._z;
	m_currentGridData.m_place.m_spaceInfo._translation = offset;
	m_currentGridData.m_place.m_spaceInfo._rotation = rot;
	m_currentGridData.m_place.m_spaceInfo._scale = scale;

	strin >> tag;
	if (tag != L"@end_place")
		return false;
	return true;
}

bool CGridDataManager::ParseStretchingBody(wstringstream& strin)
{
	CGridShapeOp::SCap capData;
	CGridShapeOp::SStretchingBody bodyData;
	SpatialInfo spaceData;

	Vector3df pos;
	Vector3df rot;
	Vector3df scale;

	wstring materialName, tag;
	bool reverse = false;
	int cnt, num;

	strin >> cnt;
	for (int i = 0; i < cnt; ++i)
	{
		strin >> reverse >> materialName >> num >> tag;
		bodyData.spaceInfo.clear();
		bodyData.reverse = reverse;
		bodyData.materialName = materialName.c_str();

		if (tag == L"index")
		{
			unsigned int idx;
			strin >> idx;

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				spaceData._translation = pos;
				spaceData._rotation = rot;
				spaceData._scale = scale;
				bodyData.spaceInfo.push_back(spaceData);
			}

			if (!OperateCurGridData(idx, EGRIDDATA_OP_STRETCH, capData, bodyData))
			{
				return false;
			}
		}
		else if (tag == L"range")
		{
			int contourLowerIdx, contourUpperIdx;
			strin >> contourLowerIdx >> contourUpperIdx;

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				spaceData._translation = pos;
				spaceData._rotation = rot;
				spaceData._scale = scale;
				bodyData.spaceInfo.push_back(spaceData);
			}

			for (int j = contourLowerIdx; j <= contourUpperIdx; ++j)
			{
				if (!OperateCurGridData(j, EGRIDDATA_OP_STRETCH, capData, bodyData))
				{
					return false;
				}
			}
		}
		else if (tag == L"array")
		{
			int idxNum, ignore;
			strin >> idxNum;
			

			for (int j = 0; j < idxNum; ++j)
			{
				strin >> ignore;
			}

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				spaceData._translation = pos;
				spaceData._rotation = rot;
				spaceData._scale = scale;
				bodyData.spaceInfo.push_back(spaceData);
			}

			for (int j = 0; j < idxNum; ++j)
			{
				if (!OperateCurGridData(j, EGRIDDATA_OP_STRETCH, capData, bodyData))
				{
					return false;
				}
			}
		}
		else
			return false;
	}

	strin >> tag;
	if (tag != L"@end_stretching_body")
		return false;
	return true;
}

bool CGridDataManager::ParsePolygon(wstringstream& strin)
{
	CGridPolygon poly;
	CGridPolygon::SPolyPoint polyPt;
	float a, b, degree;
	bool clockwise, firstEntry = true;
	unsigned int numRows, ignore, idx, cnt, mark = -1;
	strin >> numRows >> ignore >> m_currentGridData.m_polyBaseContourIdx;

	for (unsigned int i = 0; i < numRows; ++i)
	{
		strin >> idx >> a >> b >> degree >> cnt >> clockwise;

		if (mark != idx)
		{
			mark = idx;
			if (firstEntry)
			{
				firstEntry = false;
			}
			else
			{
				m_currentGridData.m_polygons.push_back(poly);
				poly.m_polyPts.clear();
			}
		}

		polyPt.m_idx = idx;
		polyPt.m_pos.set(a, b);
		polyPt.m_degree = degree;
		polyPt.m_cnt = cnt;
		polyPt.m_clockwise = clockwise;
		poly.m_polyPts.push_back(polyPt);
	}

	if (poly.m_polyPts.size() > 0)
	{
		m_currentGridData.m_polygons.push_back(poly);
	}

	wstring tag;
	strin >> tag;
	if (tag != L"@end_polygon")
		return false;
	return true;
}

bool CGridDataManager::ParseCircle(wstringstream& strin)
{
	CGridCircle circle;
	float a, b, diameter;
	bool clockwise;
	unsigned int numRows, ignore, cnt;
	strin >> numRows >> ignore >> m_currentGridData.m_circBaseContourIdx;

	for (unsigned int i = 0; i < numRows; ++i)
	{
		strin >> diameter >> a >> b >> cnt >> clockwise;
		circle.m_diameter = diameter;
		circle.m_pos.set(a, b);
		circle.m_cnt = cnt;
		circle.m_clockwise = clockwise;
		m_currentGridData.m_circles.push_back(circle);
	}

	wstring tag;
	strin >> tag;
	if (tag != L"@end_circle")
		return false;
	return true;
}

bool CGridDataManager::ParseGridding(wstringstream& strin)
{
	if (!ParseProp(strin))
		return false;

	wstring tag;
	while (true)
	{
		strin >> tag;

		if (tag == L"@place")
		{
			tag.clear();
			if (!ParsePlace(strin))
				return false;
		}
		else if (tag == L"@polygon")
		{
			tag.clear();
			if (!ParsePolygon(strin))
				return false;
		}
		else if (tag == L"@circle")
		{
			tag.clear();
			if (!ParseCircle(strin))
				return false;
		}
		else
			break;
	}

	return ParseCapAndBody(strin, tag);
}

bool CGridDataManager::OperateCurGridData(unsigned idx, EGRIDDATA_OP opType, const CGridShapeOp::SCap& capData, const CGridShapeOp::SStretchingBody& bodyData)
{
	if (idx == -1)
	{
		switch (opType)
		{
		case EGRIDDATA_OP_TOPCAP:
			m_currentGridData.m_isTopCapTessAll = true;
			m_currentGridData.m_topCap = capData;
			break;
		case EGRIDDATA_OP_BOTTOMCAP:
			m_currentGridData.m_isBottomCapTessAll = true;
			m_currentGridData.m_bottomCap = capData;
			break;
		default:
			return false;
		}
		return true;
	}

	if (idx >= m_currentGridData.m_circBaseContourIdx && idx < m_currentGridData.m_circBaseContourIdx + m_currentGridData.m_circles.size())
	{
		switch (opType)
		{
		case EGRIDDATA_OP_TOPCAP:
			m_currentGridData.m_circles[idx - m_currentGridData.m_circBaseContourIdx].m_shapeOp.m_topCaps = capData;
			break;
		case EGRIDDATA_OP_BOTTOMCAP:
			m_currentGridData.m_circles[idx - m_currentGridData.m_circBaseContourIdx].m_shapeOp.m_bottomCaps = capData;
			break;
		case EGRIDDATA_OP_STRETCH:
			m_currentGridData.m_circles[idx - m_currentGridData.m_circBaseContourIdx].m_shapeOp.m_stretchingBodies = bodyData;
			break;
		default:
			return false;
		}
		return true;
	}
	else if (idx >= m_currentGridData.m_polyBaseContourIdx && idx < m_currentGridData.m_polyBaseContourIdx + m_currentGridData.m_polygons.size())
	{
		int polyIdxOffset = 0;
		switch (opType)
		{
		case EGRIDDATA_OP_TOPCAP:
			m_currentGridData.m_polygons[idx - m_currentGridData.m_polyBaseContourIdx].m_shapeOp.m_topCaps = capData;
			break;
		case EGRIDDATA_OP_BOTTOMCAP:
			m_currentGridData.m_polygons[idx - m_currentGridData.m_polyBaseContourIdx].m_shapeOp.m_bottomCaps = capData;
			break;
		case EGRIDDATA_OP_STRETCH:
			m_currentGridData.m_polygons[idx - m_currentGridData.m_polyBaseContourIdx].m_shapeOp.m_stretchingBodies = bodyData;
			break;
		default:
			return false;
		}
		return true;
	}

	return false;
}
