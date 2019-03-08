/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 图片管理器
\*************************************************************************/
#ifndef __VAYO2D_PICTURE_MANAGER_H__
#define __VAYO2D_PICTURE_MANAGER_H__

#include "Vayo2dSupport.h"
#include "Vayo2dPicture.h"
NS_VAYO2D_BEGIN

class _Vayo2dExport PictureManager
{
	DISALLOW_COPY_AND_ASSIGN(PictureManager)
public:
	PictureManager();
	~PictureManager();
	bool       init();
	PicturePtr getPicture(const wstring& filePath);
	void       removePicture(const wstring& name);
	void       removePicture(PicturePtr picture);

private:
	map<wstring, PicturePtr> _picturePool;
};

NS_VAYO2D_END

#endif // __VAYO2D_PICTURE_MANAGER_H__
