#include "VayoDynLibManager.h"
#include "VayoDynLib.h"

NS_VAYO_BEGIN

DynLibManager::DynLibManager()
{
}

DynLibManager::~DynLibManager()
{
	map<wstring, DynLib*>::iterator it = _dynLibList.begin();
	for (; it != _dynLibList.end(); ++it)
	{
		it->second->unload();
		delete it->second;
	}
	_dynLibList.clear();
}

DynLib* DynLibManager::load(const wstring& filename)
{
	map<wstring, DynLib*>::iterator it = _dynLibList.find(filename);
	if (it != _dynLibList.end())
		return it->second;

	DynLib* pLib = new DynLib(filename);
	if (pLib->load())
	{
		_dynLibList[pLib->getName()] = pLib;
		return pLib;
	}

	SAFE_DELETE(pLib);
	return NULL;
}

void DynLibManager::unload(DynLib* lib)
{
	map<wstring, DynLib*>::iterator it = _dynLibList.find(lib->getName());
	if (it != _dynLibList.end())
		_dynLibList.erase(it);

	lib->unload();
	delete lib;
}

NS_VAYO_END
