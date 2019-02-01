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
	DisplayList(const wstring& name) : _name(name) {}
	virtual ~DisplayList() {}
	virtual void newList() = 0;
	virtual void endList(const wstring& lastMaterialName = L"") = 0;
protected:
	PROPERTY_R_REF(wstring, _name, Name)
};

NS_VAYO3D_END

#endif // __VAYO3D_DISPLAY_LIST_H__
