/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格拆分处理器
\*************************************************************************/
#ifndef __VAYO3D_TESS_GRID_HANDLER_H__
#define __VAYO3D_TESS_GRID_HANDLER_H__

#include "Vayo3dManualObject.h"
#include "VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport TessGridHandler
{
public:
	TessGridHandler();
	TessGridHandler(const wstring& filename);
	TessGridHandler(unsigned int vertexCount, unsigned int contourCount);
	~TessGridHandler();
	void reset(int vertexSize = 0, int contourSize = 0);
	bool parseTessgridFile(const wstring& filename, bool fullPath = false);
	bool parseTessgridFile(wstringstream& filestream);
	bool tesselating(ManualObject* tessObj, bool reverse = false, const wstring& materialName = L"");
	bool tesselatingOnce(ManualObject* tessObj, unsigned int contourIdx, bool reverse = false, const wstring& materialName = L"");
	void transformVertexList(const Matrix4x4& posMat, const Matrix4x4& normMat);
	void transformPositionList(const Matrix4x4& posMat, unsigned int contourIdx = -1);
	void transformNormalList(const Matrix4x4& normMat, unsigned int contourIdx = -1);
	void changeNormalList(const Vector3df& newNorm, unsigned int contourIdx = -1);
	vector<VertIdxPair>& getVertexList();
	const vector<VertIdxPair>& getVertexList() const;
	vector<unsigned int>& getContour(unsigned int listIdx);
	const vector<unsigned int>& getContour(unsigned int listIdx) const;
	unsigned int getContoursCount() const;
	vector<vector<unsigned int>>& getContourList();
	const vector<vector<unsigned int>>& getContourList() const;
	bool beginStretch(ManualObject* stretchDstObj, unsigned int contourSrcIdx, bool reverse = false, const wstring& materialName = L"");
	void stretching(const Matrix4x4& transform);
	void endStretch();
	bool filterAdjoinSamePoint(vector<Vector2df>& outPts, const vector<Vector2df>& intPts, bool eraseCollinearPoint = true);
	bool filterAdjoinSamePoint(vector<Vector3df>& outPts, const vector<Vector3df>& intPts, bool eraseCollinearPoint = true);
	bool gainCircleCenter(Position2df& ptO, const Position2df& ptA, const Position2df& ptB, float degree, bool clockwise);
	bool gainPolylineBisector(vector<Vector2df>& outBisector, const vector<Vector2df>& inPolyLine);
	bool gainFrenetFrame(vector<Matrix4x4>& outFrames, const vector<Vector3df>& inTrackLine);
	void generateArc(vector<Position3df>& outPts, const Position3df& startPt, const Position3df& rotAxisThroughPt, const Vector3df& rotAxisDirect, float arcAngle, int ptNum);
	void generatePipe(vector<Position3df>& centerTrackLine, ManualObject* pipelineDstObj, unsigned int contourSrcIdx, bool reverse = false, bool closed = false, const wstring& materialName = L"");
	void bezierCurve3Order(vector<Vector3df>& outPts, const Vector3df& anchor1, const Vector3df& control1, const Vector3df& control2, const Vector3df& anchor2, int numSteps = 30);
	void bezierCurveNOrder(vector<Vector2df>& outPts, int outCnt, const vector<Vector2df>& inPts);

private:
	void initTesselator();
	DISALLOW_COPY_AND_ASSIGN(TessGridHandler)
	friend class TessGridParser;
	Tesselator*                  _tesselator;
	ManualObject*                _opDstObj;
	wstring                      _materialName;
	unsigned int                 _contourSrcIdx;
	vector<VertIdxPair>          _vertexList;
	vector<vector<unsigned int>> _contourList;
	list<VertIdxPair>            _combineVertices;
};

NS_VAYO3D_END

#endif // __VAYO3D_TESS_GRID_HANDLER_H__
