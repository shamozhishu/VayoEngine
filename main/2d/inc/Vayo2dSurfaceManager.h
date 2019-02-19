/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 表面管理器
\*************************************************************************/
#ifndef __VAYO2D_SURFACE_MANAGER_H__
#define __VAYO2D_SURFACE_MANAGER_H__

#include "Vayo2dSupport.h"
#include "Vayo2dSurface.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport SurfaceManager
{
	DISALLOW_COPY_AND_ASSIGN(SurfaceManager)
public:
	SurfaceManager();
	~SurfaceManager();
	bool       init();
	SurfacePtr getSurface(const wstring& filePath);
	void       removeSurface(const wstring& name);
	void       removeSurface(SurfacePtr surface);

private:
	map<wstring, SurfacePtr> _surfacePool;
};

NS_VAYO2D_END

#endif // __VAYO2D_SURFACE_MANAGER_H__
