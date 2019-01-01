/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 动态链接库
\*************************************************************************/
#ifndef __VAYO_DYNLIB_H__
#define __VAYO_DYNLIB_H__

#include "VayoSupport.h"

#ifdef _WIN32
#define DYNLIB_HANDLE       hInstance
#define DYNLIB_LOAD(a)      LoadLibrary(a)
#define DYNLIB_GETSYM(a, b) GetProcAddress(a, b)
#define DYNLIB_UNLOAD(a)    !FreeLibrary(a)
struct HINSTANCE__;
typedef struct HINSTANCE__* hInstance;
#endif

NS_VAYO_BEGIN

class _VayoExport DynLib
{
public:
	DynLib(const wstring& name);
	~DynLib();
	bool  load();
	bool  unload();
	const wstring& getName(void) const { return _name; }
	void* getSymbol(const string& strName) const;

protected:
	wstring dynlibError();
protected:
	wstring       _name;
	DYNLIB_HANDLE _hInst;
};

NS_VAYO_END

#endif // __VAYO_DYNLIB_H__
