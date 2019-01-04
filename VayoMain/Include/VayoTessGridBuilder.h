/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格拆分生成器
\*************************************************************************/
#ifndef __VAYO_TESS_GRID_BUILDER_H__
#define __VAYO_TESS_GRID_BUILDER_H__

#include "VayoSupport.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"
#include "VayoNode.h"
NS_VAYO_BEGIN

class _VayoExport TessGridBuilder
{
public:
	TessGridBuilder();
	~TessGridBuilder();
	enum EPlace {
		EP_XY, EP_XZ, EP_YZ
	};
	wstringstream& getStream();
	void save(const wstring& filename, bool append = true, bool fullPath = false);
	void rebuild();
	void setProp(const wstring& modelName, const wstring& materialName=L"");
	void setPlace(EPlace place, const Vector3df& pos=Vector3df::Origin, const Vector3df& rot=Vector3df::Origin, const Vector3df& scale=Vector3df(1,1,1));
	void setTopCap(int idx, Vector3df norm, wstring materialName = L"", bool reverse = false);
	void setTopCap(int lowerIdx, int upperIdx, Vector3df norm, wstring materialName = L"", bool reverse = false);
	void setTopCap(const vector<int>& indexes, Vector3df norm, wstring materialName = L"", bool reverse = false);
	void setBottomCap(int idx, Vector3df norm, wstring materialName = L"", bool reverse = false);
	void setBottomCap(int lowerIdx, int upperIdx, Vector3df norm, wstring materialName = L"", bool reverse = false);
	void setBottomCap(const vector<int>& indexes, Vector3df norm, wstring materialName = L"", bool reverse = false);
	int addCircle(float diameter, Vector2df pos, unsigned int cnt = 60, bool clockwise = false);
	int beginAddPoly();
	void addPolyPoint(Vector2df pos, float degree=0.0f, unsigned int cnt=1, bool clockwise=false);
	void endAddPoly();
	void beginAddStretBody(int idx, wstring materialName = L"", bool reverse = false);
	void beginAddStretBody(int lowerIdx, int upperIdx, wstring materialName = L"", bool reverse = false);
	void beginAddStretBody(const vector<int>& indexes, wstring materialName=L"", bool reverse=false);
	void addSingleStret(const Vector3df& translation, const Vector3df& rot=Vector3df::Origin, const Vector3df& scale=Vector3df(1, 1, 1));
	void endAddStretBody();

private:
	void building();
	DISALLOW_COPY_AND_ASSIGN(TessGridBuilder)

	typedef struct tagProp
	{
		wstring modelName;
		wstring materialName;
		unsigned int vertCnt;
		unsigned int contourCnt;
	} Prop;

	typedef struct tagPlace
	{
		EPlace place;
		SpatialInfo spaceInfo;
	} Place;

	typedef struct tagCircle
	{
		float diameter;
		Vector2df pos;
		unsigned int cnt;
		bool clockwise;
	} Circle;

	typedef struct tagPolygon
	{
		unsigned int idx;
		Vector2df pos;
		float degree;
		unsigned int cnt;
		bool clockwise;
	} Polygon;

	typedef struct tagCap
	{
		bool reverse;
		wstring materialName;
		Vector3df normal;
		wstring idxType;
		vector<int> indexes;
	} Cap;

	typedef struct tagStretchingBody
	{
		bool reverse;
		wstring materialName;
		vector<SpatialInfo> spaceInfo;
		wstring idxType;
		vector<int> indexes;
	} StretchingBody;

private:
	wstringstream          _stream;
	Prop                   _prop;
	Place                  _place;
	vector<Circle>         _circles;
	vector<Polygon>        _polygons;
	vector<Cap>            _topCaps;
	vector<Cap>            _bottomCaps;
	vector<StretchingBody> _stretchingBodies;
	bool                   _hasBuildStream;
	unsigned int           _polyContourIdx;
};

NS_VAYO_END

#endif // __VAYO_TESS_GRID_BUILDER_H__
