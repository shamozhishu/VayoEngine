#include "VayoTessGridBuilder.h"
#include "VayoConfigManager.h"
#include "VayoRoot.h"
#include "VayoUtils.h"
#include "VayoFileIO.h"

NS_VAYO_BEGIN

void TessGridBuilder::building()
{
	unsigned int vertexIdxOffset = 0;
	unsigned int contourIdxOffset = 0;

	// gridding
	_stream << L"#gridding\r\n";

	// prop
	_stream << L"@prop\r\n";
	_stream << _prop.modelName << L" " << _prop.materialName << L" "
		<< _prop.vertCnt << L" " << _prop.contourCnt << L"\r\n";
	_stream << L"@end_prop\r\n";

	// place
	_stream << L"@place\r\n";
	switch (_place.place)
	{
	case EP_XZ: _stream << L"xz "; break;
	case EP_YZ: _stream << L"yz "; break;
	default: _stream << L"xy "; break;
	}
	_stream << _place.spaceInfo._translation._x << L" "
		<< _place.spaceInfo._translation._y << L" "
		<< _place.spaceInfo._translation._z << L" "
		<< _place.spaceInfo._rotation._x << L" "
		<< _place.spaceInfo._rotation._y << L" "
		<< _place.spaceInfo._rotation._z << L" "
		<< _place.spaceInfo._scale._x << L" "
		<< _place.spaceInfo._scale._y << L" "
		<< _place.spaceInfo._scale._z << L"\r\n";
	_stream << L"@end_place\r\n";

	// circle
	unsigned int num = _circles.size();
	if (num > 0)
	{
		_stream << L"@circle\r\n";
		_stream << num << L" " << vertexIdxOffset << L" " << contourIdxOffset << L"\r\n";;
		for (unsigned int i = 0; i < num; ++i)
		{
			_stream << _circles[i].diameter << L" "
				<< _circles[i].pos._x << L" "
				<< _circles[i].pos._y << L" "
				<< _circles[i].cnt << L" "
				<< _circles[i].clockwise << L"\r\n";
			vertexIdxOffset += _circles[i].cnt;
			contourIdxOffset += 1;
		}
		_stream << L"@end_circle\r\n";
	}

	// polygon
	num = _polygons.size();
	if (num > 0)
	{
		_stream << L"@polygon\r\n";
		_stream << num << L" " << vertexIdxOffset << L" " << contourIdxOffset << L"\r\n";
		for (unsigned int i = 0; i < num; ++i)
		{
			_stream << _polygons[i].idx << L" "
				<< _polygons[i].pos._x << L" "
				<< _polygons[i].pos._y << L" "
				<< _polygons[i].degree << L" "
				<< _polygons[i].cnt << L" "
				<< _polygons[i].clockwise << L"\r\n";
		}
		_stream << L"@end_polygon\r\n";
	}

	// top_cap
	num = _topCaps.size();
	if (num > 0)
	{
		_stream << L"@top_cap\r\n";
		_stream << num << L"\r\n";
		wstring strTmp;

		for (unsigned int i = 0; i < num; ++i)
		{
			_stream << _topCaps[i].reverse << L" "
				<< _topCaps[i].materialName << L" "
				<< _topCaps[i].normal._x << L" "
				<< _topCaps[i].normal._y << L" "
				<< _topCaps[i].normal._z << L" "
				<< _topCaps[i].idxType << L" ";

			strTmp = _topCaps[i].idxType;
			if (strTmp == L"index")
				_stream << _topCaps[i].indexes[0] << L"\r\n";
			else if (strTmp == L"range")
				_stream << _topCaps[i].indexes[0] << L" " << _topCaps[i].indexes[1] << L"\r\n";
			else if (strTmp == L"array")
			{
				_stream << _topCaps[i].indexes.size();
				vector<int>::iterator it = _topCaps[i].indexes.begin();
				for (; it != _topCaps[i].indexes.end(); ++it)
					_stream << L" " << (*it);
				_stream << L"\r\n";
			}
		}
		_stream << L"@end_cap\r\n";
	}

	// stretching_body
	num = _stretchingBodies.size();
	if (num > 0)
	{
		_stream << L"@stretching_body\r\n";
		_stream << num << L"\r\n";
		wstring strTmp;

		for (unsigned int i = 0; i < num; ++i)
		{
			unsigned int stretchNum = _stretchingBodies[i].spaceInfo.size();
			_stream << _stretchingBodies[i].reverse << L" "
				<< _stretchingBodies[i].materialName << L" "
				<< stretchNum << L" "
				<< _stretchingBodies[i].idxType << L" ";
			strTmp = _stretchingBodies[i].idxType;
			if (strTmp == L"index")
				_stream << _stretchingBodies[i].indexes[0] << L"\r\n";
			else if (strTmp == L"range")
				_stream << _stretchingBodies[i].indexes[0] << L" " << _stretchingBodies[i].indexes[1] << L"\r\n";
			else if (strTmp == L"array")
			{
				_stream << _stretchingBodies[i].indexes.size();
				vector<int>::iterator it = _stretchingBodies[i].indexes.begin();
				for (; it != _stretchingBodies[i].indexes.end(); ++it)
					_stream << L" " << (*it);
				_stream << L"\r\n";
			}
			for (unsigned j = 0; j < stretchNum; ++j)
			{
				_stream << _stretchingBodies[i].spaceInfo[j]._translation._x << L" "
					<< _stretchingBodies[i].spaceInfo[j]._translation._y << L" "
					<< _stretchingBodies[i].spaceInfo[j]._translation._z << L" "
					<< _stretchingBodies[i].spaceInfo[j]._rotation._x << L" "
					<< _stretchingBodies[i].spaceInfo[j]._rotation._y << L" "
					<< _stretchingBodies[i].spaceInfo[j]._rotation._z << L" "
					<< _stretchingBodies[i].spaceInfo[j]._scale._x << L" "
					<< _stretchingBodies[i].spaceInfo[j]._scale._y << L" "
					<< _stretchingBodies[i].spaceInfo[j]._scale._z << L"\r\n";
			}
		}
		_stream << L"@end_stretching_body\r\n";
	}

	// bottom_cap
	num = _bottomCaps.size();
	if (num > 0)
	{
		_stream << L"@bottom_cap\r\n";
		_stream << num << L"\r\n";
		wstring strTmp;

		for (unsigned int i = 0; i < num; ++i)
		{
			_stream << _bottomCaps[i].reverse << L" "
				<< _bottomCaps[i].materialName << L" "
				<< _bottomCaps[i].normal._x << L" "
				<< _bottomCaps[i].normal._y << L" "
				<< _bottomCaps[i].normal._z << L" "
				<< _bottomCaps[i].idxType << L" ";

			strTmp = _bottomCaps[i].idxType;
			if (strTmp == L"index")
				_stream << _bottomCaps[i].indexes[0] << L"\r\n";
			else if (strTmp == L"range")
				_stream << _bottomCaps[i].indexes[0] << L" " << _bottomCaps[i].indexes[1] << L"\r\n";
			else if (strTmp == L"array")
			{
				_stream << _bottomCaps[i].indexes.size();
				vector<int>::iterator it = _bottomCaps[i].indexes.begin();
				for (; it != _bottomCaps[i].indexes.end(); ++it)
					_stream << L" " << (*it);
				_stream << L"\r\n";
			}
		}
		_stream << L"@end_cap\r\n";
	}

	// end_gridding
	_stream << L"#end_gridding\r\n";
	_hasBuildStream = true;
}

TessGridBuilder::TessGridBuilder()
	: _hasBuildStream(false)
	, _polyContourIdx(0)
{
	_prop.vertCnt = 0;
	_prop.contourCnt = 0;
}

TessGridBuilder::~TessGridBuilder()
{
}

wstringstream& TessGridBuilder::getStream()
{
	if (!_hasBuildStream)
		building();
	return _stream;
}

void TessGridBuilder::save(const wstring& filename, bool append /*= true*/, bool fullPath /*= false*/)
{
	wstring filePath, fileName;
	fileName = filename;
	trim(fileName);
	size_t curpos = fileName.rfind(L'.');
	if (curpos == wstring::npos || fileName.substr(curpos) != L".tessgrid")
		fileName += L".tessgrid";

	if (fullPath)
		filePath = fileName;
	else
		filePath = Root::getSingleton().getConfigManager()->getSceneConfig().ModelsPath + fileName;

	WriteFile fout(filePath, append);
	string strtmp = w2a_(getStream().str());
	fout.write(strtmp.data(), strtmp.size() * sizeof(char));
}

void TessGridBuilder::rebuild()
{
	if (_hasBuildStream)
	{
		_hasBuildStream = false;
		_stream.str(L"");
		_stream.clear();
		_polyContourIdx = 0;
		_prop.modelName.clear();
		_prop.materialName.clear();
		_prop.vertCnt = 0;
		_prop.contourCnt = 0;
		_place.place = EP_XY;
		_place.spaceInfo._translation = Vector3df::Origin;
		_place.spaceInfo._rotation = Vector3df::Origin;
		_place.spaceInfo._scale.set(1.0f, 1.0f, 1.0f);
		_circles.clear();
		_polygons.clear();
		_topCaps.clear();
		_bottomCaps.clear();
		_stretchingBodies.clear();
	}
}

void TessGridBuilder::setProp(const wstring& modelName, const wstring& materialName/*=L""*/)
{
	_prop.modelName = modelName;
	_prop.materialName = materialName.empty() ? L"default_material" : materialName;
}

void TessGridBuilder::setPlace(EPlace place, const Vector3df& pos/*=Vector3df::Origin*/, const Vector3df& rot/*=Vector3df::Origin*/, const Vector3df& scale/*=Vector3df(1,1,1)*/)
{
	_place.place = place;
	_place.spaceInfo._translation = pos;
	_place.spaceInfo._rotation = rot;
	_place.spaceInfo._scale = scale;
}

void TessGridBuilder::setTopCap(int idx, Vector3df norm, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	Cap cap;
	cap.idxType = L"index";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes.push_back(idx);
	_topCaps.push_back(cap);
}

void TessGridBuilder::setTopCap(int lowerIdx, int upperIdx, Vector3df norm, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	Cap cap;
	cap.idxType = L"range";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes.push_back(lowerIdx);
	cap.indexes.push_back(upperIdx);
	_topCaps.push_back(cap);
}

void TessGridBuilder::setTopCap(const vector<int>& indexes, Vector3df norm, wstring materialName/*=L""*/, bool reverse/*=false*/)
{
	Cap cap;
	cap.idxType = L"array";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes = indexes;
	_topCaps.push_back(cap);
}

void TessGridBuilder::setBottomCap(int idx, Vector3df norm, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	Cap cap;
	cap.idxType = L"index";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes.push_back(idx);
	_bottomCaps.push_back(cap);
}

void TessGridBuilder::setBottomCap(int lowerIdx, int upperIdx, Vector3df norm, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	Cap cap;
	cap.idxType = L"range";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes.push_back(lowerIdx);
	cap.indexes.push_back(upperIdx);
	_bottomCaps.push_back(cap);
}

void TessGridBuilder::setBottomCap(const vector<int>& indexes, Vector3df norm, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	Cap cap;
	cap.idxType = L"array";
	cap.normal = norm;
	cap.materialName = materialName.empty() ? L"default_material" : materialName;
	cap.reverse = reverse;
	cap.indexes = indexes;
	_bottomCaps.push_back(cap);
}

int TessGridBuilder::addCircle(float diameter, Vector2df pos, unsigned int cnt/*=60*/, bool clockwise/*=false*/)
{
	Circle cir;
	cir.diameter = diameter;
	cir.pos = pos;
	cir.cnt = cnt;
	cir.clockwise = clockwise;
	_circles.push_back(cir);
	_prop.vertCnt += cnt;
	int curContour = (int)_prop.contourCnt;
	_prop.contourCnt += 1;
	return curContour;
}

int TessGridBuilder::beginAddPoly()
{
	int curContour = (int)_prop.contourCnt;
	_prop.contourCnt += 1;
	return curContour;
}

void TessGridBuilder::addPolyPoint(Vector2df pos, float degree/*=0.0f*/, unsigned int cnt/*=1*/, bool clockwise/*=false*/)
{
	Polygon poly;
	poly.idx = _polyContourIdx;
	poly.pos = pos;
	poly.degree = degree;
	poly.cnt = cnt;
	poly.clockwise = clockwise;
	_polygons.push_back(poly);

	if (degree <= 0.0f || degree >= 360.0f)
		_prop.vertCnt += 1;
	else
		_prop.vertCnt += cnt;
}

void TessGridBuilder::endAddPoly()
{
	++_polyContourIdx;
}

void TessGridBuilder::beginAddStretBody(int idx, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	_stretchingBodies.push_back(StretchingBody());
	StretchingBody& stretBody = _stretchingBodies.back();
	stretBody.idxType = L"index";
	stretBody.indexes.push_back(idx);
	stretBody.materialName = materialName.empty() ? L"default_material" : materialName;
	stretBody.reverse = reverse;
}

void TessGridBuilder::beginAddStretBody(int lowerIdx, int upperIdx, wstring materialName /*= L""*/, bool reverse /*= false*/)
{
	_stretchingBodies.push_back(StretchingBody());
	StretchingBody& stretBody = _stretchingBodies.back();
	stretBody.idxType = L"range";
	stretBody.indexes.push_back(lowerIdx);
	stretBody.indexes.push_back(upperIdx);
	stretBody.materialName = materialName.empty() ? L"default_material" : materialName;
	stretBody.reverse = reverse;
}

void TessGridBuilder::beginAddStretBody(const vector<int>& indexes, wstring materialName/*=L""*/, bool reverse/*=false*/)
{
	_stretchingBodies.push_back(StretchingBody());
	StretchingBody& stretBody = _stretchingBodies.back();
	stretBody.idxType = L"array";
	stretBody.indexes = indexes;
	stretBody.materialName = materialName.empty() ? L"default_material" : materialName;
	stretBody.reverse = reverse;
}

void TessGridBuilder::addSingleStret(const Vector3df& translation, const Vector3df& rot/*=Vector3df::Origin*/, const Vector3df& scale/*=Vector3df(1, 1, 1)*/)
{
	StretchingBody& stretBody = _stretchingBodies.back();
	SpatialInfo spaceInfo;
	spaceInfo._translation = translation;
	spaceInfo._rotation = rot;
	spaceInfo._scale = scale;
	stretBody.spaceInfo.push_back(spaceInfo);
}

void TessGridBuilder::endAddStretBody()
{
	// do nothing.
}

NS_VAYO_END
