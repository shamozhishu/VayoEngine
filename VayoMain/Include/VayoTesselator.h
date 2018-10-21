/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 网格细分器
\*************************************************************************/
#ifndef __VAYO_TESSELATOR_H__
#define __VAYO_TESSELATOR_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class Tesselator
{
public:
	enum ETessWinding
	{
		ETW_WINDING_ODD,
		ETW_WINDING_NONZERO,
		ETW_WINDING_POSITIVE,
		ETW_WINDING_NEGATIVE,
		ETW_WINDING_ABS_GEQ_TWO
	};
	typedef void(*TessBeginDataProc)   (unsigned int, void*);
	typedef void(*TessVertexDataProc)  (void*, void*);
	typedef void(*TessEndDataProc)     (void*);
	typedef void(*TessErrorDataProc)   (unsigned int, void*);
	typedef void(*TessCombineDataProc) (double[3], void*[4], float[4], void**, void*);
public:
	Tesselator(const wstring& name) : _name(name) {}
	virtual ~Tesselator() {}
	virtual void          beginPolygon(void* data) = 0;
	virtual void          beginContour() = 0;
	virtual void          vertex(Vertex* vert) = 0;
	virtual void          endContour() = 0;
	virtual void          endPolygon() = 0;
	virtual void          setBeginCallback(TessBeginDataProc pBegin) = 0;
	virtual void          setVertexCallback(TessVertexDataProc pVertex) = 0;
	virtual void          setEndCallback(TessEndDataProc pEnd) = 0;
	virtual void          setErrorCallback(TessErrorDataProc pError) = 0;
	virtual void          setCombineCallback(TessCombineDataProc pCombine) = 0;
	virtual void          setWindingProperty(ETessWinding value) = 0;
protected:
	PROPERTY_R_REF(wstring, _name, Name)
};

NS_VAYO_END

#endif // __VAYO_TESSELATOR_H__
