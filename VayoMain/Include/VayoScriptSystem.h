/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 脚本系统
\*************************************************************************/
#ifndef __VAYO_SCRIPT_SYSTEM_H__
#define __VAYO_SCRIPT_SYSTEM_H__

#include "VayoSupport.h"
NS_VAYO_BEGIN

enum EScriptType
{
	EST_NONE = 0,
	EST_LUA,
	EST_JAVASCRIPT
};

class _VayoExport ScriptProtocol
{
public:
	ScriptProtocol(const wstring& name);
	virtual ~ScriptProtocol() {}
	virtual bool executeString(const string& codes) = 0;
	virtual bool executeScriptFile(const wstring& filename) = 0;
	virtual int  executeGlobalFunction(const string& functionName) = 0;
	virtual int  executeFunctionByHandler(int nHandler, int numArgs) = 0;
	virtual void removeScriptHandler(int nHandler) {};
	virtual int  reallocateScriptHandler(int nHandler) { return -1; }
	virtual void call_va(const string& func, const string& sig, ...) {}

protected:
	PROPERTY_R(wstring, _scriptName, ScriptName)
};

class _VayoExport ScriptSystem
{
public:
	ScriptSystem();
	virtual ~ScriptSystem();
	virtual EScriptType getScriptType() const;
	virtual ScriptProtocol* createScript(const wstring& name = L"") = 0;
	virtual ScriptProtocol* findScript(const wstring& name) const;
	virtual void destroyScript(ScriptProtocol* script);
	virtual void destroyScript(const wstring& name);
	virtual void destroyScripts();

protected:
	map<wstring, ScriptProtocol*> _scripts;
};

NS_VAYO_END

#endif // __VAYO_SCRIPT_SYSTEM_H__
