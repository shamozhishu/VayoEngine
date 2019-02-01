/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* Lua�ű�����ջ
\*************************************************************************/
#ifndef __LUA_SCRIPT_STACK_H__
#define __LUA_SCRIPT_STACK_H__

#include "LuaSupport.h"

class LuaScriptStack : public ScriptProtocol
{
public:
	LuaScriptStack(const wstring& name, LuaScriptSystem* scriptSys);
	~LuaScriptStack();

	bool executeString(const string& codes);
	bool executeScriptFile(const wstring& filename);
	int  executeGlobalFunction(const string& functionName);
	int  executeFunctionByHandler(int nHandler, int numArgs);
	void removeScriptHandler(int nHandler);
	int  reallocateScriptHandler(int nHandler);
	void call_va(const string& func, const string& sig, ...);

	lua_State* getLuaState() const;
	int  executeFunction(int numArgs);
	void clean();
	void pushInt(int intValue);
	void pushFloat(float floatValue);
	void pushBoolean(bool boolValue);
	void pushString(const char* stringValue);
	void pushString(const char* stringValue, int length);
	void pushNil(void);
	bool pushFunctionByHandler(int nHandler);

protected:
	lua_State* _state;
	LuaScriptSystem* _scriptSys;
};

#endif // __LUA_SCRIPT_STACK_H__
