/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 网格拆分解析器
\*************************************************************************/
#ifndef __VAYO3D_TESS_GRID_PARSER_H__
#define __VAYO3D_TESS_GRID_PARSER_H__

#include "Vayo3dTessGridBuilder.h"
NS_VAYO3D_BEGIN

class TessGridParser
{
	friend class TessGridHandler;
	static void __stdcall tessBeginDataCB(EPrimitiveType which, TessGridHandler* tess);
	static void __stdcall tessVertexDataCB(Vertex* vert, TessGridHandler* tess);
	static void __stdcall tessEndDataCB(TessGridHandler* tess);
	static void __stdcall tessCombineDataCB(const double newVertex[3], const Vertex* neighborVertex[4], const float neighborWeight[4], Vertex** outData, TessGridHandler* tess);
	//----------------------------------------------------------------------------------
	//##解析.tessgrid格式模型文件##
	//----------------------------------------------------------------------------------
	static ManualObject* parseProp(wstringstream& strin, TessGridHandler& tess);
	static bool parseCapAndBody(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj, const wstring& tag);
	static bool onTesselating(TessGridHandler& tess, ManualObject* pObj, const Vector3df& norm, const wstring& materialname, bool reverse, int idx);
	static bool parseCap(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj);
	static bool parsePlace(wstringstream& strin, TessGridHandler& tess, TessGridBuilder::EPlace& place, Matrix4x4& mat);
	static bool parseStretchingBody(wstringstream& strin, TessGridHandler& tess, ManualObject* pObj);
	static bool parsePolygon(wstringstream& strin, TessGridHandler& tess, const TessGridBuilder::EPlace& place, const Matrix4x4& mat);
	static bool parseCircle(wstringstream& strin, TessGridHandler& tess, const TessGridBuilder::EPlace& place, const Matrix4x4& mat);
	static bool parseGridding(wstringstream& strin, TessGridHandler& tess);
};

NS_VAYO3D_END

#endif // __VAYO3D_TESS_GRID_PARSER_H__
