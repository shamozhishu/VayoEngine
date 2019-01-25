#include "Vayo3dTessGridHandler.h"
#include "Vayo3dTessGridBuilder.h"
#include "Vayo3dTessGridParser.h"
#include "Vayo3dTesselator.h"
#include "VayoConfigManager.h"
#include "Vayo3dRenderSystem.h"
#include "Vayo3dSceneManager.h"
#include "math/VayoLine2d.h"
#include "Vayo3dRoot.h"
#include "VayoUtils.h"
#include "VayoLog.h"

NS_VAYO3D_BEGIN

void TessGridHandler::initTesselator()
{
	_tesselator = Root::getSingleton().getActiveRenderer()->createTesselator();
	_tesselator->setBeginCallback((Tesselator::TessBeginDataProc)TessGridParser::tessBeginDataCB);
	_tesselator->setVertexCallback((Tesselator::TessVertexDataProc)TessGridParser::tessVertexDataCB);
	_tesselator->setEndCallback((Tesselator::TessEndDataProc)TessGridParser::tessEndDataCB);
	_tesselator->setCombineCallback((Tesselator::TessCombineDataProc)TessGridParser::tessCombineDataCB);
}

TessGridHandler::TessGridHandler()
	: _tesselator(NULL)
	, _opDstObj(NULL)
{
	initTesselator();
}

TessGridHandler::TessGridHandler(const wstring& filename)
	: _tesselator(NULL)
	, _opDstObj(NULL)
{
	initTesselator();
	parseTessgridFile(filename);
}

TessGridHandler::TessGridHandler(unsigned int vertexCount, unsigned int contourCount)
	: _tesselator(NULL)
	, _opDstObj(NULL)
{
	initTesselator();
	_vertexList.resize(vertexCount);
	_contourList.resize(contourCount);
}

TessGridHandler::~TessGridHandler()
{
	Root::getSingleton().getActiveRenderer()->destroyTesselator(_tesselator->getName());
}

void TessGridHandler::reset(int vertexSize /*= 0*/, int contourSize /*= 0*/)
{
	_vertexList.clear();
	_contourList.clear();
	if (vertexSize > 0)
		_vertexList.resize((size_t)vertexSize);
	if (contourSize > 0)
		_contourList.resize((size_t)contourSize);
}

bool TessGridHandler::parseTessgridFile(const wstring& filename, bool fullPath /*= false*/)
{
	wstring fileName = filename;
	trim(fileName);
	if (fileName != L"" && fileName.substr(fileName.rfind(L'.')) == L".tessgrid")
	{
		wstring filePath;
		if (fullPath)
			filePath = fileName;
		else
			filePath = ConfigManager::getSingleton().getSceneConfig().ModelsPath + fileName;

		std::wifstream fin(filePath);
		if (!fin)
		{
			Log::wprint(ELL_ERROR, L"文件[%s]打开失败", filePath.c_str());
			return false;
		}

		fin.seekg(0, ios_base::end);
		if ((int)fin.tellg() == 0)
		{
			Log::wprint(ELL_ERROR, L"文件[%s]为空", filePath.c_str());
			return false;
		}

		fin.seekg(0, ios_base::beg);
		fin.imbue(locale("chs"));
		wstringstream filestream;
		filestream << fin.rdbuf();
		
		if (parseTessgridFile(filestream))
		{
			Log::wprint(ELL_INFORMATION, L"文件[%s]解析成功", filePath.c_str());
			return true;
		}
		
		Log::wprint(ELL_ERROR, L"文件[%s]解析失败", filePath.c_str());
	}

	return false;
}

bool TessGridHandler::parseTessgridFile(wstringstream& filestream)
{
	_vertexList.clear();
	_contourList.clear();
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
			succParse = TessGridParser::parseGridding(filestream, *this);
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

bool TessGridHandler::tesselating(ManualObject* tessObj, bool reverse /*= false*/, const wstring& materialName /*= L""*/)
{
	if (NULL == tessObj)
		return false;

	unsigned int verticesNum = _vertexList.size();
	unsigned int contoursNum = _contourList.size();
	if (0 == verticesNum || 0 == contoursNum)
	{
		Log::wprint(ELL_WARNING, L"verticesNum=%d,contoursNum=%d(TessGridBuilder::tesselating)", verticesNum, contoursNum);
		return false;
	}

	_opDstObj = tessObj;
	_materialName = materialName;
	_tesselator->beginPolygon(this);

	if (reverse)
	{
		vector<vector<unsigned int>>::const_reverse_iterator cit = _contourList.crbegin();
		for (; cit != _contourList.crend(); ++cit)
		{
			const vector<unsigned int>& curContour = *cit;
			unsigned int aContourVertNum = curContour.size();
			if (aContourVertNum > 0)
			{
				_tesselator->beginContour();
				vector<unsigned int>::const_reverse_iterator citor = curContour.crbegin();
				for (; citor != curContour.crend(); ++citor)
					_tesselator->vertex(&_vertexList[*citor]);
				_tesselator->endContour();
			}
		}
	}
	else
	{
		vector<vector<unsigned int>>::const_iterator cit = _contourList.cbegin();
		for (; cit != _contourList.cend(); ++cit)
		{
			const vector<unsigned int>& curContour = *cit;
			unsigned int aContourVertNum = curContour.size();
			if (aContourVertNum > 0)
			{
				_tesselator->beginContour();
				vector<unsigned int>::const_iterator citor = curContour.cbegin();
				for (; citor != curContour.cend(); ++citor)
					_tesselator->vertex(&_vertexList[*citor]);
				_tesselator->endContour();
			}
		}
	}

	_combineVertices.clear();
	_tesselator->endPolygon();
	_combineVertices.clear();

	return true;
}

bool TessGridHandler::tesselatingOnce(ManualObject* tessObj, unsigned int contourIdx, bool reverse /*= false*/,
	const wstring& materialName /*= L""*/)
{
	if (NULL == tessObj)
		return false;

	unsigned int verticesNum = _vertexList.size();
	unsigned int contoursNum = _contourList.size();
	if (0 == verticesNum || 0 == contoursNum)
	{
		Log::wprint(ELL_WARNING, L"verticesNum=%d,contoursNum=%d(TessGridBuilder::tesselatingOnce)", verticesNum, contoursNum);
		return false;
	}

	if (contourIdx >= contoursNum)
	{
		Log::wprint(ELL_WARNING, L"contourIdx>=%d(TessGridBuilder::tesselatingOnce)", contoursNum);
		return false;
	}

	const vector<unsigned int>& curContour = _contourList[contourIdx];
	unsigned int aContourVertNum = curContour.size();
	if (aContourVertNum == 0)
	{
		Log::wprint(ELL_WARNING, L"aContourVertNum==0(TessGridBuilder::tesselatingOnce)");
		return false;
	}

	_opDstObj = tessObj;
	_materialName = materialName;
	_tesselator->beginPolygon(this);

	if (reverse)
	{
		_tesselator->beginContour();
		vector<unsigned int>::const_reverse_iterator citor = curContour.crbegin();
		for (; citor != curContour.crend(); ++citor)
			_tesselator->vertex(&_vertexList[*citor]);
		_tesselator->endContour();
	}
	else
	{
		_tesselator->beginContour();
		vector<unsigned int>::const_iterator citor = curContour.cbegin();
		for (; citor != curContour.cend(); ++citor)
			_tesselator->vertex(&_vertexList[*citor]);
		_tesselator->endContour();
	}

	_combineVertices.clear();
	_tesselator->endPolygon();
	_combineVertices.clear();

	return true;
}

void TessGridHandler::transformVertexList(const Matrix4x4& posMat, const Matrix4x4& normMat)
{
	if (posMat == IdentityMatrix && normMat == IdentityMatrix)
		return;

	vector<Vertex>::iterator it = _vertexList.begin();
	for (; it != _vertexList.end(); ++it)
	{
		Vertex& vert = *it;
		if (posMat != IdentityMatrix)
			posMat.transformVect(vert._position);
		if (normMat != IdentityMatrix)
			normMat.transformVect(vert._normal);
	}
}

void TessGridHandler::transformPositionList(const Matrix4x4& posMat, unsigned int contourIdx /*= -1*/)
{
	if (posMat == IdentityMatrix)
		return;

	if (contourIdx < _contourList.size())
	{
		vector<unsigned int>::iterator it = _contourList[contourIdx].begin();
		for (; it != _contourList[contourIdx].end(); ++it)
			posMat.transformVect(_vertexList[*it]._position);
	}
	else
		transformVertexList(posMat, IdentityMatrix);
}

void TessGridHandler::transformNormalList(const Matrix4x4& normMat, unsigned int contourIdx /*= -1*/)
{
	if (normMat == IdentityMatrix)
		return;

	if (contourIdx < _contourList.size())
	{
		vector<unsigned int>::iterator it = _contourList[contourIdx].begin();
		for (; it != _contourList[contourIdx].end(); ++it)
			normMat.transformVect(_vertexList[*it]._normal);
	}
	else
		transformVertexList(IdentityMatrix, normMat);
}

void TessGridHandler::changeNormalList(const Vector3df& newNorm, unsigned int contourIdx /*= -1*/)
{
	if (contourIdx < _contourList.size())
	{
		vector<unsigned int>::const_iterator cit = _contourList[contourIdx].cbegin();
		for (; cit != _contourList[contourIdx].cend(); ++cit)
			_vertexList[*cit]._normal = newNorm;
	}
	else
	{
		vector<vector<unsigned int>>::const_iterator cit = _contourList.cbegin();
		for (; cit != _contourList.cend(); ++cit)
		{
			const vector<unsigned int>& curContour = *cit;
			unsigned int aContourVertNum = curContour.size();
			if (aContourVertNum > 0)
			{
				vector<unsigned int>::const_iterator citor = curContour.cbegin();
				for (; citor != curContour.cend(); ++citor)
					_vertexList[*citor]._normal = newNorm;
			}
		}
	}
}

vector<Vertex>& TessGridHandler::getVertexList()
{
	return _vertexList;
}

const vector<Vertex>& TessGridHandler::getVertexList() const
{
	return _vertexList;
}

vector<unsigned int>& TessGridHandler::getContour(unsigned int listIdx)
{
	if (listIdx >= _contourList.size())
	{
		Log::wprint(ELL_ERROR, L"索引下标出界(TessGridBuilder::getContour).");
		return _contourList[0];
	}
	return _contourList[listIdx];
}

const vector<unsigned int>& TessGridHandler::getContour(unsigned int listIdx) const
{
	if (listIdx >= _contourList.size())
	{
		Log::wprint(ELL_ERROR, L"索引下标出界(TessGridBuilder::getContour).");
		return _contourList[0];
	}
	return _contourList[listIdx];
}

unsigned int TessGridHandler::getContoursCount() const
{
	return _contourList.size();
}

vector<vector<unsigned int>>& TessGridHandler::getContourList()
{
	return _contourList;
}

const vector<vector<unsigned int>>& TessGridHandler::getContourList() const
{
	return _contourList;
}

bool TessGridHandler::beginStretch(ManualObject* stretchDstObj, unsigned int contourSrcIdx,
	bool reverse /*= false*/, const wstring& materialName /*= L""*/)
{
	if (NULL == stretchDstObj)
		return false;

	if (contourSrcIdx >= _contourList.size())
	{
		Log::wprint(ELL_WARNING, L"轮廓列表下标出界(TessGridBuilder::beginStretch).");
		return false;
	}

	unsigned int vertNum = _vertexList.size();
	if (0 == vertNum)
	{
		Log::wprint(ELL_WARNING, L"源顶点列表为空(TessGridBuilder::beginStretch)");
		return false;
	}

	const vector<unsigned int>& curContour = _contourList[contourSrcIdx];
	unsigned int contourVertNum = curContour.size();
	if (0 == contourVertNum)
	{
		Log::wprint(ELL_WARNING, L"轮廓顶点数为空(TessGridBuilder::beginStretch)");
		return false;
	}

	Vertex tmpVert;
	stretchDstObj->begin(EPT_TRIANGLES, materialName);

	if (reverse)
	{
		vector<unsigned int>::const_reverse_iterator citor = curContour.crbegin();
		for (; citor != curContour.crend(); ++citor)
		{
			tmpVert = _vertexList[*citor];
			stretchDstObj->colour(tmpVert._color);
			stretchDstObj->textureCoord(tmpVert._texCoord);
			stretchDstObj->normal(tmpVert._normal);
			stretchDstObj->position(tmpVert._position);
		}
	}
	else
	{
		vector<unsigned int>::const_iterator citor = curContour.cbegin();
		for (; citor != curContour.cend(); ++citor)
		{
			tmpVert = _vertexList[*citor];
			stretchDstObj->colour(tmpVert._color);
			stretchDstObj->textureCoord(tmpVert._texCoord);
			stretchDstObj->normal(tmpVert._normal);
			stretchDstObj->position(tmpVert._position);
		}
	}

	stretchDstObj->getOpSubMesh()->clearIndexList();
	_opDstObj = stretchDstObj;
	_contourSrcIdx = contourSrcIdx;
	return true;
}

void TessGridHandler::stretching(const Matrix4x4& transform)
{
	VAYO_ASSERT(_opDstObj != NULL);
	SubMesh* pSideSubMesh = _opDstObj->getOpSubMesh();
	vector<Vertex> newVertsList = pSideSubMesh->getVertexList();
	unsigned int vertNum = pSideSubMesh->getVertexCount();
	unsigned int triNum = _contourList[_contourSrcIdx].size();
	if (triNum > vertNum)
		return;

	Vertex tmpVert;
	vector<Vertex> transformedVertList;
	for (unsigned int i = 0; i < triNum; ++i)
	{
		tmpVert = newVertsList[vertNum - triNum + i];
		if (triNum != vertNum)
			newVertsList.push_back(tmpVert);
		transform.transformVect(tmpVert._position);
		transformedVertList.push_back(tmpVert);
	}

	newVertsList.insert(newVertsList.end(), transformedVertList.begin(), transformedVertList.end());
	unsigned int newVertsNum = newVertsList.size();
	unsigned int idx1, idx2, idx3;
	for (unsigned int i = 0; i < triNum; ++i)
	{
		idx1 = newVertsNum - triNum - triNum + i;
		idx2 = idx1 + triNum;
		idx3 = idx1 + 1;
		if (idx3 >= newVertsNum - triNum)
			idx3 = newVertsNum - triNum - triNum;
		_opDstObj->triangle(idx1, idx2, idx3);

		idx1 = idx3;
		idx3 = idx1 + triNum;
		_opDstObj->triangle(idx1, idx2, idx3);
	}

	pSideSubMesh->setVertexList(newVertsList);
}

void TessGridHandler::endStretch()
{
	_opDstObj->end();
	_opDstObj = NULL;
}

bool TessGridHandler::filterAdjoinSamePoint(vector<Vector2df>& outPts, const vector<Vector2df>& intPts, bool eraseCollinearPoint /*= true*/)
{
	unsigned int size = intPts.size();
	if (size == 0)
		return false;

	vector<Vector2df> tmpPts;
	for (unsigned int i = 0; i < size - 1; ++i)
	{
		if (intPts[i] != intPts[i + 1])
			tmpPts.push_back(intPts[i]);
	}

	tmpPts.push_back(intPts[size - 1]);

	if (eraseCollinearPoint)
	{
		Vector2df bisector;
		Vector2df ptA, ptB, ptC;
		size = tmpPts.size();
		if (size == 0)
			return false;

		vector<Vector2df>::iterator i = tmpPts.begin();
		for (; i != tmpPts.end();)
		{
			if (i != tmpPts.begin() && i != tmpPts.end() - 1)
			{
				ptA = *(i - 1);
				ptB = *i;
				ptC = *(i + 1);

				Vector2df lineSeg1 = ptB - ptA;
				lineSeg1.normalize();
				Vector2df lineSeg2 = ptC - ptB;
				lineSeg2.normalize();
				bisector = lineSeg1 + lineSeg2;
				if (bisector == Vector2df::Origin)
				{
					Log::wprint(ELL_WARNING, L"三点共线且构成的两个向量反向[ptA:%f,%f][ptB:%f,%f][ptC:%f,%f]",
						ptA._x, ptA._y, ptB._x, ptB._y, ptC._x, ptC._y);
					return false;
				}

				lineSeg1.invert();
				bisector = lineSeg1 + lineSeg2;
				if (bisector == Vector2df::Origin)
				{
					// 清除共线点
					i = tmpPts.erase(i);
					continue;
				}
			}

			++i;
		}
	}

	outPts = tmpPts;
	return true;
}

bool TessGridHandler::filterAdjoinSamePoint(vector<Vector3df>& outPts, const vector<Vector3df>& intPts, bool eraseCollinearPoint /*= true*/)
{
	unsigned int size = intPts.size();
	if (size == 0)
		return false;

	vector<Vector3df> tmpPts;
	for (unsigned int i = 0; i < size - 1; ++i)
	{
		if (intPts[i] != intPts[i + 1])
			tmpPts.push_back(intPts[i]);
	}

	tmpPts.push_back(intPts[size - 1]);

	if (eraseCollinearPoint)
	{
		Vector3df bisector;
		Vector3df ptA, ptB, ptC;
		size = tmpPts.size();
		if (size == 0)
			return false;

		vector<Vector3df>::iterator i = tmpPts.begin();
		for (; i != tmpPts.end();)
		{
			if (i != tmpPts.begin() && i != tmpPts.end() - 1)
			{
				ptA = *(i - 1);
				ptB = *i;
				ptC = *(i + 1);

				Vector3df lineSeg1 = ptB - ptA;
				lineSeg1.normalize();
				Vector3df lineSeg2 = ptC - ptB;
				lineSeg2.normalize();
				bisector = lineSeg1 + lineSeg2;
				if (bisector == Vector3df::Origin)
				{
					Log::wprint(ELL_WARNING, L"三点共线且构成的两个向量反向[ptA:%f,%f,%f][ptB:%f,%f,%f][ptC:%f,%f,%f]",
						ptA._x, ptA._y, ptA._z, ptB._x, ptB._y, ptB._z, ptC._x, ptC._y, ptC._z);
					return false;
				}

				lineSeg1.invert();
				bisector = lineSeg1 + lineSeg2;
				if (bisector == Vector3df::Origin)
				{
					// 清除共线点
					i = tmpPts.erase(i);
					continue;
				}
			}

			++i;
		}
	}

	outPts = tmpPts;
	return true;
}

bool TessGridHandler::gainCircleCenter(Position2df& ptO, const Position2df& ptA, const Position2df& ptB, float degree, bool clockwise)
{
	if (degree <= 0.0f || degree >= 360.0f)
		return false;

	Line2df AB(ptA, ptB);
	Vector2df tmp = AB.getVector();
	double magnitude = AB.getLength();
	if (isZero(magnitude))
		return false;

	Vector2df vertical(-tmp._y, tmp._x);
	vertical.normalize();
	Position2df ptC = AB.getMiddle();
	double lengthAC = magnitude / 2.0;
	double halfRadian = m3dDegToRad((double)degree) / 2.0;
	double lengthOC = lengthAC / tan(halfRadian);

	if (clockwise)
		vertical *= -1.0f;

	ptO = ptC + vertical * (float)lengthOC;
	return true;
}

bool TessGridHandler::gainPolylineBisector(vector<Vector2df>& outBisector, const vector<Vector2df>& inPolyLine)
{
	int size = (int)inPolyLine.size();
	if (size <= 1)
		return false;

	vector<Vector2df> tmpPolyLine;
	if (!filterAdjoinSamePoint(tmpPolyLine, inPolyLine, true))
		return false;

	size = (int)tmpPolyLine.size();
	if (size <= 1)
		return false;

	Vector2df bisector;
	Vector2df ptA, ptB, ptC;

	for (int i = 0; i < size; ++i)
	{
		if (i == 0)
		{
			ptA = tmpPolyLine[i];
			ptB = tmpPolyLine[i + 1];
			Vector2df lineSeg = ptB - ptA;
			bisector.set(-lineSeg._y, lineSeg._x);
		}
		else if (i == size - 1)
		{
			ptA = tmpPolyLine[size - 2];
			ptB = tmpPolyLine[size - 1];
			Vector2df lineSeg = ptB - ptA;
			bisector.set(-lineSeg._y, lineSeg._x);
		}
		else
		{
			ptA = tmpPolyLine[i - 1];
			ptB = tmpPolyLine[i];
			ptC = tmpPolyLine[i + 1];

			Vector2df lineSeg1 = ptB - ptA;
			lineSeg1.normalize();
			Vector2df lineSeg2 = ptC - ptB;
			lineSeg2.normalize();
			bisector = lineSeg1 + lineSeg2;
			bisector = Vector2df(-bisector._y, bisector._x);
		}

		bisector.normalize();
		outBisector.push_back(bisector);
	}

	return true;
}

bool TessGridHandler::gainFrenetFrame(vector<Matrix4x4>& outFrames, const vector<Vector3df>& inTrackLine)
{
	int size = (int)inTrackLine.size();
	if (size <= 1)
		return false;

	vector<Vector3df> tmpTrackLine;
	if (!filterAdjoinSamePoint(tmpTrackLine, inTrackLine, true))
		return false;

	size = (int)tmpTrackLine.size();
	if (size <= 1)
		return false;

	Vector3df ptA, ptB, ptC;
	Vector3df xaxis, yaxis, zaxis;
	Matrix4x4 aFrame;

	if (size == 2)
	{
		zaxis = tmpTrackLine[1] - tmpTrackLine[0];
		zaxis.normalize();

		if (zaxis != Vector3df::YAxis)
			ptA = Vector3df::YAxis;
		else
			ptA = Vector3df::ZAxis;

		xaxis = ptA.crossProduct(zaxis);
		xaxis.normalize();

		yaxis = zaxis.crossProduct(xaxis);
		yaxis.normalize();

		aFrame(0, 0) = xaxis._x;
		aFrame(1, 0) = xaxis._y;
		aFrame(2, 0) = xaxis._z;
		aFrame(3, 0) = 0;

		aFrame(0, 1) = yaxis._x;
		aFrame(1, 1) = yaxis._y;
		aFrame(2, 1) = yaxis._z;
		aFrame(3, 1) = 0;

		aFrame(0, 2) = zaxis._x;
		aFrame(1, 2) = zaxis._y;
		aFrame(2, 2) = zaxis._z;
		aFrame(3, 2) = 0;

		aFrame(0, 3) = tmpTrackLine[0]._x;
		aFrame(1, 3) = tmpTrackLine[0]._y;
		aFrame(2, 3) = tmpTrackLine[0]._z;
		aFrame(3, 3) = 1.0f;

		outFrames.push_back(aFrame);
		aFrame(0, 3) = tmpTrackLine[1]._x;
		aFrame(1, 3) = tmpTrackLine[1]._y;
		aFrame(2, 3) = tmpTrackLine[1]._z;
		outFrames.push_back(aFrame);

		return true;
	}

	for (int i = 0; i < size; ++i)
	{
		if (i == 0)
		{
			ptA = tmpTrackLine[i];
			ptB = tmpTrackLine[i + 1];
			ptC = tmpTrackLine[i + 2];
			zaxis = ptB - ptA;
			xaxis = zaxis.crossProduct(ptC - ptB);
			zaxis.normalize();
			xaxis.normalize();
			yaxis = zaxis.crossProduct(xaxis);
		}
		else if (i == size - 1)
		{
			ptA = tmpTrackLine[size - 3];
			ptB = tmpTrackLine[size - 2];
			ptC = tmpTrackLine[size - 1];
			zaxis = ptC - ptB;
			xaxis = (ptB - ptA).crossProduct(zaxis);
			zaxis.normalize();
			xaxis.normalize();
			yaxis = zaxis.crossProduct(xaxis);
		}
		else
		{
			ptA = tmpTrackLine[i - 1];
			ptB = tmpTrackLine[i];
			ptC = tmpTrackLine[i + 1];

			Vector3df lineSeg1 = ptA - ptB;
			lineSeg1.normalize();
			Vector3df lineSeg2 = ptC - ptB;
			lineSeg2.normalize();
			Vector3df tmpY = lineSeg1 + lineSeg2;

			if (tmpY.dotProduct(yaxis) < 0.0f)
				tmpY.invert();

			yaxis = tmpY;
			yaxis.normalize();
			lineSeg1.invert();
			zaxis = lineSeg1 + lineSeg2;
			zaxis.normalize();
			xaxis = yaxis.crossProduct(zaxis);
		}

		aFrame(0, 0) = xaxis._x;
		aFrame(1, 0) = xaxis._y;
		aFrame(2, 0) = xaxis._z;
		aFrame(3, 0) = 0;

		aFrame(0, 1) = yaxis._x;
		aFrame(1, 1) = yaxis._y;
		aFrame(2, 1) = yaxis._z;
		aFrame(3, 1) = 0;

		aFrame(0, 2) = zaxis._x;
		aFrame(1, 2) = zaxis._y;
		aFrame(2, 2) = zaxis._z;
		aFrame(3, 2) = 0;

		aFrame(0, 3) = tmpTrackLine[i]._x;
		aFrame(1, 3) = tmpTrackLine[i]._y;
		aFrame(2, 3) = tmpTrackLine[i]._z;
		aFrame(3, 3) = 1.0f;

		outFrames.push_back(aFrame);
	}

	return true;
}

void TessGridHandler::generateArc(vector<Position3df>& outPts, const Position3df& startPt,
	const Position3df& rotAxisThroughPt, const Vector3df& rotAxisDirect, float arcAngle, int ptNum)
{
	if (ptNum <= 0 || arcAngle <= 0.0f)
		return;

	outPts.clear();
	outPts.resize(ptNum);
	outPts[0] = startPt;
	ptNum -= 1;
	float perAngle = arcAngle / (float)ptNum;
	Matrix4x4 rotMat;
	rotMat.setRotationDegrees(perAngle, rotAxisDirect);
	Matrix4x4 translateMat;
	translateMat.setTranslation(rotAxisThroughPt);
	Matrix4x4 translateInvertMat;
	translateInvertMat.setTranslation(rotAxisThroughPt * -1);
	Matrix4x4 finalTransform = translateMat * rotMat * translateInvertMat;

	Position3df tmpPt = startPt;
	for (int i = 0; i < ptNum; ++i)
	{
		finalTransform.transformVect(tmpPt);
		outPts[i + 1] = tmpPt;
	}
}

void TessGridHandler::generatePipe(vector<Position3df>& centerTrackLine, ManualObject* pipelineDstObj,
	unsigned int contourSrcIdx, bool reverse /*= false*/, bool closed /*= false*/, const wstring& materialName /*= L""*/)
{
	unsigned int size = centerTrackLine.size();
	unsigned int triNum = _contourList[contourSrcIdx].size();
	if (size <= 1 || triNum <= 2)
		return;

	if (!filterAdjoinSamePoint(centerTrackLine, centerTrackLine, true))
		return;

	size = centerTrackLine.size();
	if (size <= 1)
		return;

	vector<Matrix4x4> frenetFrames;
	gainFrenetFrame(frenetFrames, centerTrackLine);
	if (frenetFrames.size() != centerTrackLine.size())
		return;

	Vertex tmpVert;
	vector<Vertex> newVertsList;
	vector<Vector3df> oriPosList;
	for (unsigned int i = 0; i < triNum; ++i)
	{
		oriPosList.push_back(_vertexList[_contourList[contourSrcIdx][i]]._position);
	}

	if (reverse)
	{
		std::reverse(oriPosList.begin(), oriPosList.end());
	}

	size = frenetFrames.size();
	for (unsigned int i = 0; i < size; ++i)
	{
		for (unsigned int j = 0; j < triNum; ++j)
		{
			frenetFrames[i].transformVect(oriPosList[j], tmpVert._position);
			tmpVert._normal = (tmpVert._position - centerTrackLine[i]).normalize();
			newVertsList.push_back(tmpVert);
		}
	}

	pipelineDstObj->begin(EPT_TRIANGLES, materialName);

	unsigned int vertOffset = 0;
	unsigned int idx1, idx2, idx3;
	for (unsigned int i = 0; i < size - 1; ++i)
	{
		for (unsigned int j = 0; j < triNum; ++j)
		{
			idx1 = vertOffset + j;
			idx2 = idx1 + triNum;
			idx3 = idx1 + 1;
			if (idx3 >= vertOffset + triNum)
				idx3 = vertOffset;
			pipelineDstObj->triangle(idx1, idx2, idx3);

			idx1 = idx3;
			idx3 = idx1 + triNum;
			pipelineDstObj->triangle(idx1, idx2, idx3);
		}

		vertOffset += triNum;
	}

	if (closed)
	{
		vector<Vertex> cap1;
		cap1.insert(cap1.end(), newVertsList.begin(), newVertsList.begin() + triNum);

		vector<Vertex> cap2;
		cap2.insert(cap2.end(), newVertsList.end() - triNum, newVertsList.end());

		newVertsList.insert(newVertsList.end(), cap1.begin(), cap1.end());
		newVertsList.insert(newVertsList.end(), cap2.begin(), cap2.end());

		for (unsigned int j = 0; j < triNum - 2; ++j)
		{
			idx1 = vertOffset;
			idx2 = j + vertOffset + 1;
			idx3 = j + vertOffset + 2;
			if (reverse)
				std::swap(idx2, idx3);
			pipelineDstObj->triangle(idx1, idx2, idx3);
			newVertsList[idx1]._normal = newVertsList[idx2]._normal = newVertsList[idx3]._normal
				= (centerTrackLine[0] - centerTrackLine[1]).normalize();
		}

		vertOffset += triNum;

		for (unsigned int j = 0; j < triNum - 2; ++j)
		{
			idx1 = vertOffset;
			idx2 = j + vertOffset + 2;
			idx3 = j + vertOffset + 1;
			if (reverse)
				std::swap(idx2, idx3);
			pipelineDstObj->triangle(idx1, idx2, idx3);
			newVertsList[idx1]._normal = newVertsList[idx2]._normal = newVertsList[idx3]._normal
				= (centerTrackLine[size - 1] - centerTrackLine[size - 2]).normalize();
		}
	}

	pipelineDstObj->getOpSubMesh()->setVertexList(newVertsList);
	pipelineDstObj->end();
}

void TessGridHandler::bezierCurve3Order(vector<Vector3df>& outPts, const Vector3df& anchor1, const Vector3df& control1,
	const Vector3df& control2, const Vector3df& anchor2, int numSteps /*= 30*/)
{
	outPts.push_back(anchor1);

	float subdiv_step = 1.0f / (numSteps + 1);
	float subdiv_step2 = subdiv_step*subdiv_step;
	float subdiv_step3 = subdiv_step*subdiv_step*subdiv_step;

	float pre1 = 3.0f * subdiv_step;
	float pre2 = 3.0f * subdiv_step2;
	float pre4 = 6.0f * subdiv_step2;
	float pre5 = 6.0f * subdiv_step3;

	Vector3df tmp1 = anchor1 - control1 * 2.0f + control2;
	Vector3df tmp2 = (control1 - control2)*3.0f - anchor1 + anchor2;
	Vector3df f = anchor1;
	Vector3df df = (control1 - anchor1)*pre1 + tmp1*pre2 + tmp2*subdiv_step3;
	Vector3df ddf = tmp1*pre4 + tmp2*pre5;
	Vector3df dddf = tmp2*pre5;

	int step = numSteps;
	while (step--)
	{
		f += df;
		df += ddf;
		ddf += dddf;
		outPts.push_back(f);
	}

	outPts.push_back(anchor2);
}

static inline Vector2df bezier_interpolation_func(float t, const vector<Vector2df>& points)
{
	int count = points.size();
	VAYO_ASSERT(count > 0);
	vector<Vector2df> tmp_points;
	tmp_points.resize(count);
	for (int i = 1; i < count; ++i)
	{
		for (int j = 0; j < count - i; ++j)
		{
			if (i == 1)
			{
				tmp_points[j]._x = (float)(points[j]._x * (1 - t) + points[j + 1]._x * t);
				tmp_points[j]._y = (float)(points[j]._y * (1 - t) + points[j + 1]._y * t);
				continue;
			}
			tmp_points[j]._x = (float)(tmp_points[j]._x * (1 - t) + tmp_points[j + 1]._x * t);
			tmp_points[j]._y = (float)(tmp_points[j]._y * (1 - t) + tmp_points[j + 1]._y * t);
		}
	}
	return tmp_points[0];
}

void TessGridHandler::bezierCurveNOrder(vector<Vector2df>& outPts, int outCnt, const vector<Vector2df>& inPts)
{
	float step = 1.0f / outCnt;
	float t = 0;
	outPts.clear();
	outPts.resize(outCnt);
	for (int i = 0; i < outCnt; ++i)
	{
		Vector2df tempPt = bezier_interpolation_func(t, inPts);
		t += step;
		outPts[i] = tempPt;
	}
}

NS_VAYO3D_END
