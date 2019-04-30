#include "Vayo3dTessGridParser.h"
#include "Vayo3dTessGridHandler.h"
#include "Vayo3dSceneManager.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"

NS_VAYO3D_BEGIN

void __stdcall TessGridParser::tessBeginDataCB(EPrimitiveType which, TessGridHandler* tess)
{
	tess->_opDstObj->begin(which, tess->_materialName, true);
}

void __stdcall TessGridParser::tessVertexDataCB(VertIdxPair* vert, TessGridHandler* tess)
{
	tess->_opDstObj->index(vert->_idx);
}

void __stdcall TessGridParser::tessEndDataCB(TessGridHandler* tess)
{
	tess->_opDstObj->end();
}

void __stdcall TessGridParser::tessCombineDataCB(const double newVertex[3], const VertIdxPair* neighborVertex[4], const float neighborWeight[4], VertIdxPair** outData, TessGridHandler* tess)
{
	float red, green, blue, x, y, z, a, b, c, d;
	Vertex tmpVert;
	tmpVert._position.set((float)newVertex[0], (float)newVertex[1], (float)newVertex[2]);

	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._color.getRed() : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._color.getRed() : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._color.getRed() : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._color.getRed() : 0.0f;
	red = a + b + c + d;
	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._color.getGreen() : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._color.getGreen() : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._color.getGreen() : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._color.getGreen() : 0.0f;
	green = a + b + c + d;
	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._color.getBlue() : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._color.getBlue() : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._color.getBlue() : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._color.getBlue() : 0.0f;
	blue = a + b + c + d;
	tmpVert._color.set(255, red, green, blue);

	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._normal._x : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._normal._x : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._normal._x : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._normal._x : 0.0f;
	x = a + b + c + d;
	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._normal._y : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._normal._y : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._normal._y : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._normal._y : 0.0f;
	y = a + b + c + d;
	a = neighborVertex[0] ? neighborWeight[0] * neighborVertex[0]->_vert._normal._z : 0.0f;
	b = neighborVertex[1] ? neighborWeight[1] * neighborVertex[1]->_vert._normal._z : 0.0f;
	c = neighborVertex[2] ? neighborWeight[2] * neighborVertex[2]->_vert._normal._z : 0.0f;
	d = neighborVertex[3] ? neighborWeight[3] * neighborVertex[3]->_vert._normal._z : 0.0f;
	z = a + b + c + d;
	tmpVert._normal.set(x, y, z);

	SharedSubMesh* pSharedSubMesh = tess->_opDstObj->getMesh()->getSharedSubMesh();
	unsigned int idx = pSharedSubMesh->addCombineVertex(tmpVert);
	tess->_combineVertices.push_back(VertIdxPair(tmpVert, idx + pSharedSubMesh->getVertexCount()));
	*outData = &tess->_combineVertices.back();
}
//----------------------------------------------------------------------------------
//##解析.tessgrid格式模型文件##
//----------------------------------------------------------------------------------
inline ManualObject* TessGridParser::parseProp(wstringstream& strin, TessGridHandler& tess)
{
	wstring tag;
	strin >> tag;
	if (tag != L"@prop")
		return NULL;

	strin >> tag;
	SceneManager* pSceneMgr = Root::getSingleton().getCurSceneMgr();
	ManualObject* pObj = pSceneMgr->findObject<ManualObject>(tag);
	if (!pObj)
	{
		Log::wprint(ELL_ERROR, L"ManualObject:[%s]不存在", tag.c_str());
		return NULL;
	}

	strin >> tag;
	if (tag != L"default_material")
		pObj->setMaterial(tag);
	else
		pObj->setMaterial(L"");

	unsigned int vertexCount, contourCount;
	strin >> vertexCount >> contourCount;
	tess._vertexList.clear();
	tess._contourList.clear();
	tess._vertexList.resize(vertexCount);
	tess._contourList.resize(contourCount);

	strin >> tag;
	if (tag != L"@end_prop")
		return NULL;

	pObj->getMesh()->destroyAllSubMeshs();
	return pObj;
}

bool TessGridParser::parseCapAndBody(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj, const wstring& tag)
{
	wstring aTag = tag;

	if (aTag == L"@top_cap")
	{
		if (!parseCap(strin, tess, pObj))
			return false;
		strin >> aTag;
	}

	if (aTag == L"@stretching_body")
	{
		if (!parseStretchingBody(strin, tess, pObj))
			return false;
		strin >> aTag;
	}

	if (aTag == L"@bottom_cap")
	{
		if (!parseCap(strin, tess, pObj))
			return false;
		strin >> aTag;
	}

	if (aTag != L"#end_gridding")
		return false;
	return true;
}

inline bool TessGridParser::onTesselating(TessGridHandler& tess, ManualObject* pObj,
	const Vector3df& norm, const wstring& materialname, bool reverse, int idx)
{
	wstring materialName = materialname == L"default_material" ? L"" : materialname;
	bool tessSucc = false;
	if (idx < 0)
	{
		tess.changeNormalList(norm);
		tessSucc = tess.tesselating(pObj, reverse, materialName);
	}
	else
	{
		tess.changeNormalList(norm, (unsigned int)idx);
		tessSucc = tess.tesselatingOnce(pObj, (unsigned int)idx, reverse, materialName);
	}

	return tessSucc;
}

bool TessGridParser::parseCap(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj)
{
	int cnt;
	Vector3df norm;
	bool reverse;
	wstring materialname, tag;
	strin >> cnt;
	for (int i = 0; i < cnt; ++i)
	{
		strin >> reverse >> materialname >> norm._x >> norm._y >> norm._z >> tag;
		norm.normalize();

		if (tag == L"index")
		{
			int idx;
			strin >> idx;
			if (!onTesselating(tess, pObj, norm, materialname, reverse, idx))
			{
				return false;
			}
		}
		else if (tag == L"range")
		{
			int contourLowerIdx, contourUpperIdx;
			strin >> contourLowerIdx >> contourUpperIdx;

			if (contourLowerIdx >= (int)tess._contourList.size() || contourUpperIdx >= (int)tess._contourList.size())
				return false;

			for (int idx = contourLowerIdx; idx <= contourUpperIdx; ++idx)
			{
				if (!onTesselating(tess, pObj, norm, materialname, reverse, idx))
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
				if (!onTesselating(tess, pObj, norm, materialname, reverse, idx))
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

bool TessGridParser::parsePlace(wstringstream& strin, TessGridHandler& tess, TessGridBuilder::EPlace& place, Matrix4x4& mat)
{
	wstring tag;
	strin >> tag;
	if (tag == L"xy")
		place = TessGridBuilder::EP_XY;
	else if (tag == L"xz")
		place = TessGridBuilder::EP_XZ;
	else if (tag == L"yz")
		place = TessGridBuilder::EP_YZ;
	else
		return false;

	Vector3df offset;
	Vector3df rot;
	Vector3df scale;
	strin >> offset._x >> offset._y >> offset._z;
	strin >> rot._x >> rot._y >> rot._z;
	strin >> scale._x >> scale._y >> scale._z;
	mat.makeIdentity();
	mat.setTranslation(offset);
	mat.setRotationDegrees(rot);
	mat.setScale(scale);

	strin >> tag;
	if (tag != L"@end_place")
		return false;
	return true;
}

bool TessGridParser::parseStretchingBody(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj)
{
	Vector3df pos;
	Vector3df rot;
	Vector3df scale;
	Matrix4x4 transformMat;
	vector<Matrix4x4> transformMats;

	wstring materialName, tag;
	bool reverse = false;
	int cnt, num;

	strin >> cnt;
	for (int i = 0; i < cnt; ++i)
	{
		strin >> reverse >> materialName >> num >> tag;

		if (tag == L"index")
		{
			transformMats.clear();
			unsigned int idx;
			strin >> idx;

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				transformMat.makeIdentity();
				transformMat.setTranslation(pos);
				transformMat.setRotationDegrees(rot);
				transformMat.setScale(scale);
				transformMats.push_back(transformMat);
			}

			if (tess.beginStretch(pObj, idx, reverse, materialName))
			{
				for (int k = 0; k < num; ++k)
				{
					tess.stretching(transformMats[k]);
					tess.transformPositionList(transformMats[k], idx);
				}

				tess.endStretch();
			}
		}
		else if (tag == L"range")
		{
			int contourLowerIdx, contourUpperIdx;
			strin >> contourLowerIdx >> contourUpperIdx;

			if (contourLowerIdx < 0 || contourLowerIdx >= tess._contourList.size() ||
				contourUpperIdx < 0 || contourUpperIdx >= tess._contourList.size())
				return false;

			transformMats.clear();

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				transformMat.makeIdentity();
				transformMat.setTranslation(pos);
				transformMat.setRotationDegrees(rot);
				transformMat.setScale(scale);
				transformMats.push_back(transformMat);
			}

			for (int j = contourLowerIdx; j <= contourUpperIdx; ++j)
			{
				if (tess.beginStretch(pObj, (unsigned int)j, reverse, materialName))
				{
					for (int k = 0; k < num; ++k)
					{
						tess.stretching(transformMats[k]);
						tess.transformPositionList(transformMats[k], (unsigned int)j);
					}

					tess.endStretch();
				}
			}
		}
		else if (tag == L"array")
		{
			int idxNum, aIdx;
			strin >> idxNum;
			vector<int> indexes;
			indexes.resize(idxNum);

			for (int j = 0; j < idxNum; ++j)
			{
				strin >> aIdx;
				indexes[j] = aIdx;
			}

			transformMats.clear();

			for (int j = 0; j < num; ++j)
			{
				strin >> pos._x >> pos._y >> pos._z;
				strin >> rot._x >> rot._y >> rot._z;
				strin >> scale._x >> scale._y >> scale._z;
				transformMat.makeIdentity();
				transformMat.setTranslation(pos);
				transformMat.setRotationDegrees(rot);
				transformMat.setScale(scale);
				transformMats.push_back(transformMat);
			}

			for (int j = 0; j < idxNum; ++j)
			{
				if (tess.beginStretch(pObj, (unsigned int)indexes[j], reverse, materialName))
				{
					for (int k = 0; k < num; ++k)
					{
						tess.stretching(transformMats[k]);
						tess.transformPositionList(transformMats[k], (unsigned int)indexes[j]);
					}

					tess.endStretch();
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

bool TessGridParser::parsePolygon(wstringstream& strin, TessGridHandler& tess, const TessGridBuilder::EPlace& place, const Matrix4x4& mat)
{
	Vertex tmpVert;
	tmpVert._normal = Vector3df(0, 0, 1);
	Vector3df pos;
	Position2df ptO, ptA, ptB;
	float a, b, degree;
	bool clockwise;
	unsigned int mark = -1, idx = 0, num = 0, numRows = 0, cnt = 0;
	unsigned int vertexIdxOffset = 0, contourIdxOffset = 0;
	unsigned int contourCount = tess._contourList.size();
	unsigned int vertexCount = tess._vertexList.size();
	strin >> numRows >> vertexIdxOffset >> contourIdxOffset;

	for (unsigned int i = 0; i < numRows; ++i)
	{
		strin >> idx;
		if (idx + contourIdxOffset >= contourCount || num + vertexIdxOffset >= vertexCount)
			return false;

		strin >> a >> b >> degree >> cnt >> clockwise;

		switch (place)
		{
		case TessGridBuilder::EP_XY: pos.set(a, b, 0.0f); break;
		case TessGridBuilder::EP_XZ: pos.set(a, 0.0f, b); break;
		case TessGridBuilder::EP_YZ: pos.set(0.0f, a, b); break;
		default: return false;
		}

		bool isOk = false;
		ptB.set(a, b);

		if (mark != idx)
			mark = idx;
		else
			isOk = tess.gainCircleCenter(ptO, ptA, ptB, degree, clockwise);

		if (isOk)
		{
			float numSign = (clockwise ? -1.0f : 1.0f);
			const float angleStep = degree / cnt * numSign;
			for (unsigned int j = 1; j <= cnt; ++j)
			{
				Vector2df tmp = ptA;
				tmp.rotateBy(angleStep * j, ptO);

				switch (place)
				{
				case TessGridBuilder::EP_XY: pos.set(tmp._x, tmp._y, 0.0f); break;
				case TessGridBuilder::EP_XZ: pos.set(tmp._x, 0.0f, tmp._y); break;
				case TessGridBuilder::EP_YZ: pos.set(0.0f, tmp._x, tmp._y); break;
				default: return false;
				}

				tmpVert._position = pos;
				mat.transformVect(tmpVert._position);
				tess.getContour(idx + contourIdxOffset).push_back(num + vertexIdxOffset);
				tess.getVertexList()[num + vertexIdxOffset] = tmpVert;
				++num;
			}
		}
		else
		{
			tmpVert._position = pos;
			mat.transformVect(tmpVert._position);
			tess.getContour(idx + contourIdxOffset).push_back(num + vertexIdxOffset);
			tess.getVertexList()[num + vertexIdxOffset] = tmpVert;
			++num;
		}

		ptA = ptB;
	}

	wstring tag;
	strin >> tag;
	if (tag != L"@end_polygon")
		return false;
	return true;
}

bool TessGridParser::parseCircle(wstringstream& strin, TessGridHandler& tess, const TessGridBuilder::EPlace& place, const Matrix4x4& mat)
{
	Vertex tmpVert;
	tmpVert._normal.set(0.0f, 0.0f, 1.0f);
	Vector3df pos, tmpPos, axis;
	float diameter;
	unsigned int cnt;
	bool clockwise;

	unsigned int vertexCnt = tess._vertexList.size();
	unsigned int contourCnt = tess._contourList.size();
	unsigned int numRows = 0, vertexIdxOffset = 0, contourIdxOffset = 0;
	strin >> numRows >> vertexIdxOffset >> contourIdxOffset;

	for (unsigned int i = 0; i < numRows; ++i)
	{
		if (i + contourIdxOffset >= contourCnt)
			return false;

		strin >> diameter;
		switch (place)
		{
		case TessGridBuilder::EP_XY:
			strin >> pos._x >> pos._y;
			tmpPos.set(diameter / 2.0f, 0.0f, 0.0f);
			axis = Vector3df::ZAxis;
			break;
		case TessGridBuilder::EP_XZ:
			strin >> pos._x >> pos._z;
			tmpPos.set(0.0f, 0.0f, diameter / 2.0f);
			axis = Vector3df::YAxis;
			break;
		case TessGridBuilder::EP_YZ:
			strin >> pos._y >> pos._z;
			tmpPos.set(0.0f, diameter / 2.0f, 0.0f);
			axis = Vector3df::XAxis;
			break;
		default:
			return false;
		}

		strin >> cnt >> clockwise;

		float numSign = (clockwise ? -1.0f : 1.0f);
		const float angleStep = (VAYO_MATH_PI * 2.0f) / VayoDegToRad(cnt) * numSign;

		Matrix4x4 transformMat;
		transformMat.setRotationDegrees(angleStep, axis);

		for (unsigned int j = 0; j < cnt; ++j)
		{
			tmpVert._position = tmpPos;
			tmpVert._position += pos;
			mat.transformVect(tmpVert._position);

			if (vertexIdxOffset >= vertexCnt)
				return false;

			tess._contourList[i + contourIdxOffset].push_back(vertexIdxOffset);
			tess._vertexList[vertexIdxOffset++] = tmpVert;
			transformMat.transformVect(tmpPos);
		}
	}

	wstring tag;
	strin >> tag;
	if (tag != L"@end_circle")
		return false;
	return true;
}

bool TessGridParser::parseGridding(wstringstream& strin, TessGridHandler& tess)
{
	ManualObject* pObj = parseProp(strin, tess);
	if (!pObj)
		return false;

	TessGridBuilder::EPlace place = TessGridBuilder::EP_XY;
	Matrix4x4 mat;
	wstring tag;

	while (true)
	{
		strin >> tag;

		if (tag == L"@place")
		{
			tag.clear();
			if (!parsePlace(strin, tess, place, mat))
				return false;
		}
		else if (tag == L"@polygon")
		{
			tag.clear();
			if (!parsePolygon(strin, tess, place, mat))
				return false;
		}
		else if (tag == L"@circle")
		{
			tag.clear();
			if (!parseCircle(strin, tess, place, mat))
				return false;
		}
		else
			break;
	}

	return parseCapAndBody(strin, tess, pObj, tag);
}

NS_VAYO3D_END
