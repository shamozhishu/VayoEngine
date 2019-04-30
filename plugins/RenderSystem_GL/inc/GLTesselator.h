/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL网格细分器
\*************************************************************************/
#ifndef __GL_TESSELATOR_H__
#define __GL_TESSELATOR_H__

#include "GLSupport.h"
#include "Vayo3dTesselator.h"

class GLTesselator : public Tesselator
{
public:
	GLTesselator(const wstring& name);
	~GLTesselator();
	void          beginPolygon(void* data);
	void          beginContour();
	void          vertex(VertIdxPair* vert);
	void          endContour();
	void          endPolygon();
	void          setBeginCallback(TessBeginDataProc pBegin);
	void          setVertexCallback(TessVertexDataProc pVertex);
	void          setEndCallback(TessEndDataProc pEnd);
	void          setErrorCallback(TessErrorDataProc pError);
	void          setCombineCallback(TessCombineDataProc pCombine);
	void          setWindingProperty(ETessWinding value);
private:
	static const char*   getPrimitiveType(GLenum type);
	static void CALLBACK tessBeginCB(GLenum which);
	static void CALLBACK tessVertexCB(const VertIdxPair* vert);
	static void CALLBACK tessEndCB();
	static void CALLBACK tessErrorCB(GLenum errorCode);
private:
	GLUtesselator* _tess;
};

#endif // __GL_TESSELATOR_H__
