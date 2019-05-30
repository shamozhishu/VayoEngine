/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 显示列表
\*************************************************************************/
#ifndef __VAYO_DISPLAY_LIST_H__
#define __VAYO_DISPLAY_LIST_H__

#include "Vayo3dSupport.h"
NS_VAYO3D_BEGIN

class DisplayList
{
public:
	virtual ~DisplayList() {}
	virtual void newList() = 0;
	virtual void endList(const wstring& lastMaterialName = L"") = 0;
	virtual bool isEmpty() = 0;
	virtual bool beginDraw(EPrimitiveType primType, const wstring& materialName = L"") = 0;
	virtual void endDraw() = 0;
	virtual void position(float x, float y, float z) = 0;
	virtual void normal(float x, float y, float z) = 0;
	virtual void textureCoord(float u, float v) = 0;
	virtual void colour(int r, int g, int b, int a = 255) = 0;
};

NS_VAYO3D_END

#endif // __VAYO3D_DISPLAY_LIST_H__
