/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 控制台日志打印
\*************************************************************************/
#ifndef __VAYO_LOG_H__
#define __VAYO_LOG_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

enum ELogLevel
{
	ELL_DEBUG = 0,
	ELL_INFORMATION = 2,
	ELL_WARNING = 6,
	ELL_ERROR = 4
};

class _VayoExport Log
{
public:
	static void print(ELogLevel level, const char* szFormat, ...);
	static void wprint(ELogLevel level, const wchar_t* szFormat, ...);
	Log(bool hasConsole = true, const string& logFileName = "");
	~Log();

private:
	ofstream _fout;
	bool     _hasConsole;
};

NS_VAYO_END

#endif // __VAYO_LOG_H__
