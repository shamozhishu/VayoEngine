#include "stdafx.h"
#include "GridDataManager.h"

static CGridDataManager* g_pDataMedelMgr = NULL;

CGridDataManager& CGridDataManager::GetIns()
{
	if (!g_pDataMedelMgr)
		g_pDataMedelMgr = new CGridDataManager();
	return *g_pDataMedelMgr;
}

void CGridDataManager::Destroy()
{
	SAFE_DELETE(g_pDataMedelMgr);
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

		fin.seekg(0, ios_base::end);
		if ((int)fin.tellg() == 0)
		{
			Log::wprint(ELL_ERROR, L"文件[%s]为空", filePath.GetString());
			return false;
		}

		fin.seekg(0, ios_base::beg);
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

bool CGridDataManager::SaveTessgridFile(CString filePath)
{
	bool isAppend = false;
	list<CGridData>::const_iterator cit = m_gridDataset.cbegin();
	for (; cit != m_gridDataset.cend(); ++cit)
	{
		BuildModelData(*cit, false);
		m_gridBuilder.save(filePath.GetString(), isAppend, true);
		isAppend = true;
	}
	return isAppend;
}

bool CGridDataManager::GeneratingModel(CString modelName, bool display)
{
	SceneManager* pCurSceneMgr = Root::getSingleton().getCurSceneMgr();
	if (!pCurSceneMgr)
		return false;

	CGridData* pGridData = GetGridData(modelName);
	if (!pGridData)
		return false;

	CGridData& refGridData = *pGridData;
	if (display)
	{
		if (!refGridData.m_needUpdateModel)
			return false;
	}

	ManualObject* pObj = pCurSceneMgr->findObject<ManualObject>(refGridData.m_prop.m_modelName.GetString());
	if (!pObj)
	{
		pObj = pCurSceneMgr->createObject<ManualObject>(refGridData.m_prop.m_modelName.GetString());
		SceneNode* pSceneNode = pCurSceneMgr->findSceneNode<SceneNode>(L"Model_Root_SceneNode");
		if (!pSceneNode)
		{
			pSceneNode = pCurSceneMgr->getRootSceneNode()->createChildSceneNode(L"Model_Root_SceneNode");
		}

		pSceneNode->createChildSceneNode(pObj->getName() + L"_SceneNode")->attachObject(pObj);
	}

	HideOtherModel(pObj->getName());
	if (!refGridData.m_needUpdateModel)
		return false;

	refGridData.m_needUpdateModel = false;
	return BuildModelData(refGridData, true);
}

void CGridDataManager::ClearAllModel()
{
	SceneManager* pCurSceneMgr = Root::getSingleton().getCurSceneMgr();
	if (pCurSceneMgr)
		pCurSceneMgr->destroySceneNode(L"Model_Root_SceneNode", true);
}

CGridData* CGridDataManager::GetGridData(CString modelName)
{
	static CGridData findGridData;
	findGridData.m_prop.m_modelName = modelName;
	list<CGridData>::iterator it = std::find(m_gridDataset.begin(), m_gridDataset.end(), findGridData);
	if (it == m_gridDataset.end())
		return NULL;
	return &(*it);
}

const CGridData* CGridDataManager::GetGridData(CString modelName) const
{
	static CGridData findGridData;
	findGridData.m_prop.m_modelName = modelName;
	list<CGridData>::const_iterator cit = std::find(m_gridDataset.cbegin(), m_gridDataset.cend(), findGridData);
	if (cit == m_gridDataset.cend())
		return NULL;
	return &(*cit);
}

unsigned int CGridDataManager::GetGridDataCnt() const
{
	return m_gridDataset.size();
}

list<CGridData>& CGridDataManager::GetGridDataset()
{
	return m_gridDataset;
}

const std::list<CGridData>& CGridDataManager::GetGridDataset() const
{
	return m_gridDataset;
}

bool CGridDataManager::BuildModelData(const CGridData& gridData, bool isBuildManulObject)
{
	m_gridBuilder.rebuild();
	m_gridBuilder.setProp(gridData.m_prop.m_modelName.GetString(), gridData.m_prop.m_materialName.GetString());

	TessGridBuilder::EPlace place = TessGridBuilder::EP_XY;
	if (gridData.m_place.m_plane == _T("XZ平面"))
		place = TessGridBuilder::EP_XZ;
	else if (gridData.m_place.m_plane == _T("YZ平面"))
		place = TessGridBuilder::EP_YZ;

	m_gridBuilder.setPlace(place, gridData.m_place.m_spaceInfo._translation,
		gridData.m_place.m_spaceInfo._rotation, gridData.m_place.m_spaceInfo._scale);

	list<CGridCircle>::const_iterator itCircle = gridData.m_circles.cbegin();
	for (; itCircle != gridData.m_circles.cend(); ++itCircle)
	{
		const CGridCircle& refCir = *itCircle;
		int idx = m_gridBuilder.addCircle(refCir.m_diameter, refCir.m_pos, refCir.m_cnt, refCir.m_clockwise);

		if (refCir.m_shapeOp.m_hasTopCaps)
		{
			m_gridBuilder.setTopCap(idx, refCir.m_shapeOp.m_topCap.normal,
				refCir.m_shapeOp.m_topCap.materialName.GetString(), refCir.m_shapeOp.m_topCap.reverse);
		}

		if (refCir.m_shapeOp.m_stretchingBodies.spaceInfo.size() > 0)
		{
			m_gridBuilder.beginAddStretBody(idx, refCir.m_shapeOp.m_stretchingBodies.materialName.GetString(), refCir.m_shapeOp.m_stretchingBodies.reverse);
			list<SpatialInfo>::const_iterator itSpatial = refCir.m_shapeOp.m_stretchingBodies.spaceInfo.begin();
			for (; itSpatial != refCir.m_shapeOp.m_stretchingBodies.spaceInfo.end(); ++itSpatial)
			{
				const SpatialInfo& spaceInfo = *itSpatial;
				m_gridBuilder.addSingleStret(spaceInfo._translation, spaceInfo._rotation, spaceInfo._scale);
			}
			m_gridBuilder.endAddStretBody();
		}

		if (refCir.m_shapeOp.m_hasBottomCaps)
		{
			m_gridBuilder.setBottomCap(idx, refCir.m_shapeOp.m_bottomCap.normal,
				refCir.m_shapeOp.m_bottomCap.materialName.GetString(), refCir.m_shapeOp.m_bottomCap.reverse);
		}
	}

	list<CGridPolygon>::const_iterator itPoly = gridData.m_polygons.cbegin();
	for (; itPoly != gridData.m_polygons.cend(); ++itPoly)
	{
		const CGridPolygon& refPoly = *itPoly;
		int idx = m_gridBuilder.beginAddPoly();
		list<CGridPolygon::SPolyPoint>::const_iterator itPt = refPoly.m_polyPts.begin();
		for (; itPt != refPoly.m_polyPts.end(); ++itPt)
		{
			const CGridPolygon::SPolyPoint& refPt = *itPt;
			m_gridBuilder.addPolyPoint(refPt.m_pos, refPt.m_degree, refPt.m_cnt, refPt.m_clockwise);
		}
		m_gridBuilder.endAddPoly();

		if (refPoly.m_shapeOp.m_hasTopCaps)
		{
			m_gridBuilder.setTopCap(idx, refPoly.m_shapeOp.m_topCap.normal,
				refPoly.m_shapeOp.m_topCap.materialName.GetString(), refPoly.m_shapeOp.m_topCap.reverse);
		}

		if (refPoly.m_shapeOp.m_stretchingBodies.spaceInfo.size() > 0)
		{
			m_gridBuilder.beginAddStretBody(idx, refPoly.m_shapeOp.m_stretchingBodies.materialName.GetString(), refPoly.m_shapeOp.m_stretchingBodies.reverse);
			list<SpatialInfo>::const_iterator itSpatial = refPoly.m_shapeOp.m_stretchingBodies.spaceInfo.begin();
			for (; itSpatial != refPoly.m_shapeOp.m_stretchingBodies.spaceInfo.end(); ++itSpatial)
			{
				const SpatialInfo& spaceInfo = *itSpatial;
				m_gridBuilder.addSingleStret(spaceInfo._translation, spaceInfo._rotation, spaceInfo._scale);
			}
			m_gridBuilder.endAddStretBody();
		}

		if (refPoly.m_shapeOp.m_hasBottomCaps)
		{
			m_gridBuilder.setBottomCap(idx, refPoly.m_shapeOp.m_bottomCap.normal,
				refPoly.m_shapeOp.m_bottomCap.materialName.GetString(), refPoly.m_shapeOp.m_bottomCap.reverse);
		}
	}

	if (gridData.m_isTopCapTessAll)
	{
		m_gridBuilder.setTopCap(-1, gridData.m_topCap.normal, gridData.m_topCap.materialName.GetString(), gridData.m_topCap.reverse);
	}

	if (gridData.m_isBottomCapTessAll)
	{
		m_gridBuilder.setBottomCap(-1, gridData.m_bottomCap.normal, gridData.m_bottomCap.materialName.GetString(), gridData.m_bottomCap.reverse);
	}

	return isBuildManulObject && m_gridHandler.parseTessgridFile(m_gridBuilder.getStream());
}

bool CGridDataManager::ParseTessgridFile(wstringstream& filestream)
{
	bool succParse = false;
	wstring tag;
	list<CGridData> gridDataset;

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
			gridDataset.push_back(m_currentGridData);
			m_currentGridData.ClearAll();
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
	else
		m_gridDataset = gridDataset;

	m_currentGridData.ClearAll();
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
		capData.reverse = reverse;
		capData.materialName = materialname.c_str();
		capData.normal = norm;

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
		m_currentGridData.m_place.m_plane = _T("XY平面");
	else if (tag == L"xz")
		m_currentGridData.m_place.m_plane = _T("XZ平面");
	else if (tag == L"yz")
		m_currentGridData.m_place.m_plane = _T("YZ平面");
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

void CGridDataManager::HideOtherModel(const wstring& excludedModel /*= L""*/)
{
	SceneManager* pCurSceneMgr = Root::getSingleton().getCurSceneMgr();
	if (!pCurSceneMgr)
		return;

	SceneNode* pSceneNode = pCurSceneMgr->findSceneNode<SceneNode>(L"Model_Root_SceneNode");
	if (pSceneNode)
	{
		const list<Node*>& nodes = pSceneNode->getChildren();
		list<Node*>::const_iterator cit = nodes.cbegin();
		for (; cit != nodes.cend(); ++cit)
		{
			Node* pChild = *cit;
			if (pChild->getName() != excludedModel + L"_SceneNode")
				pChild->setCanVisit(false);
			else
				pChild->setCanVisit(true);
		}
	}
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
		list<CGridCircle>::iterator it = m_currentGridData.m_circles.begin();
		std::advance(it, idx - m_currentGridData.m_circBaseContourIdx);

		switch (opType)
		{
		case EGRIDDATA_OP_TOPCAP:
			if (it != m_currentGridData.m_circles.end())
			{
				CGridCircle& gridCircle = (*it);
				gridCircle.m_shapeOp.m_hasTopCaps = true;
				gridCircle.m_shapeOp.m_topCap = capData;
			}
			break;
		case EGRIDDATA_OP_BOTTOMCAP:
			if (it != m_currentGridData.m_circles.end())
			{
				CGridCircle& gridCircle = (*it);
				gridCircle.m_shapeOp.m_hasBottomCaps = true;
				gridCircle.m_shapeOp.m_bottomCap = capData;
			}
			break;
		case EGRIDDATA_OP_STRETCH:
			if (it != m_currentGridData.m_circles.end())
			{
				CGridCircle& gridCircle = (*it);
				gridCircle.m_shapeOp.m_stretchingBodies = bodyData;
			}
			break;
		default:
			return false;
		}
		return true;
	}
	else if (idx >= m_currentGridData.m_polyBaseContourIdx && idx < m_currentGridData.m_polyBaseContourIdx + m_currentGridData.m_polygons.size())
	{
		list<CGridPolygon>::iterator it = m_currentGridData.m_polygons.begin();
		std::advance(it, idx - m_currentGridData.m_polyBaseContourIdx);

		switch (opType)
		{
		case EGRIDDATA_OP_TOPCAP:
			if (it != m_currentGridData.m_polygons.end())
			{
				CGridPolygon& gridPoly = (*it);
				gridPoly.m_shapeOp.m_hasTopCaps = true;
				gridPoly.m_shapeOp.m_topCap = capData;
			}
			break;
		case EGRIDDATA_OP_BOTTOMCAP:
			if (it != m_currentGridData.m_polygons.end())
			{
				CGridPolygon& gridPoly = (*it);
				gridPoly.m_shapeOp.m_hasBottomCaps = true;
				gridPoly.m_shapeOp.m_bottomCap = capData;
			}
			break;
		case EGRIDDATA_OP_STRETCH:
			if (it != m_currentGridData.m_polygons.end())
			{
				CGridPolygon& gridPoly = (*it);
				gridPoly.m_shapeOp.m_stretchingBodies = bodyData;
			}
			break;
		default:
			return false;
		}
		return true;
	}

	return false;
}
