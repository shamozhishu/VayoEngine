/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* OpenGL网格细分器
\*************************************************************************/
#ifndef __VAYO_GL_TESSELATOR_H__
#define __VAYO_GL_TESSELATOR_H__

#include "VayoGLSupport.h"
#include "VayoTesselator.h"
NS_VAYO_BEGIN

class GLTesselator : public Tesselator
{
public:
	GLTesselator(const wstring& name);
	~GLTesselator();
	void          beginPolygon(void* data);
	void          beginContour();
	void          vertex(Vertex* vert);
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
	static void CALLBACK tessVertexCB(const Vertex* vert);
	static void CALLBACK tessEndCB();
	static void CALLBACK tessErrorCB(GLenum errorCode);
private:
	GLUtesselator* _tess;
};

NS_VAYO_END

#endif // __VAYO_GL_TESSELATOR_H__
