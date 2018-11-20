/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2018 authored by ��Ӻ�
* ��̬���ӿ������
\*************************************************************************/
#ifndef __VAYO_DYNLIB_MANAGER_H__
#define __VAYO_DYNLIB_MANAGER_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

class _VayoExport DynLibManager
{
	DISALLOW_COPY_AND_ASSIGN(DynLibManager)
public:
	DynLibManager();
	virtual ~DynLibManager();
	DynLib* load(const wstring& filename);
	void    unload(DynLib* lib);

protected:
	map<wstring, DynLib*> _dynLibList;
};

NS_VAYO_END

#endif // __VAYO_DYNLIB_MANAGER_H__
