#include "VayoDynLib.h"
#include "VayoLog.h"

#ifdef _WIN32
#include <Windows.h>
#endif

NS_VAYO_BEGIN

void* DynLib::getSymbol(const string& strName) const
{
	return (void*)DYNLIB_GETSYM(_hInst, strName.c_str());
}

DynLib::DynLib(const wstring& name)
	: _name(name)
	, _hInst(NULL)
{
}

DynLib::~DynLib()
{
}

bool DynLib::load()
{
	Log::wprint(ELL_DEBUG, L"Loading library %s", _name.c_str());
	wstring name = _name;
	_hInst = (DYNLIB_HANDLE)DYNLIB_LOAD(name.c_str());
	if (!_hInst)
	{
		Log::wprint(ELL_ERROR, L"Could not load dynamic library %s. System Error:%s(DynLib::load)",
			_name.c_str(), dynlibError().c_str());
		return false;
	}
	return true;
}

bool DynLib::unload()
{
	Log::wprint(ELL_DEBUG, L"Unloading library %s", _name.c_str());
	if (DYNLIB_UNLOAD(_hInst))
	{
		Log::wprint(ELL_ERROR, L"Could not unload dynamic library %s. System Error:%s(DynLib::unload)",
			_name.c_str(), dynlibError().c_str());
		return false;
	}
	return true;
}

wstring DynLib::dynlibError()
{
#ifdef _WIN32
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf,
		0,
		NULL);
	wstring ret = (wchar_t*)lpMsgBuf;
	LocalFree(lpMsgBuf);
	return ret;
#endif
}

NS_VAYO_END
