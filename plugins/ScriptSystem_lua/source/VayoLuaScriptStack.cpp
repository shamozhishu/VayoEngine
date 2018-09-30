#include "VayoLuaScriptStack.h"
#include "VayoLuaScriptSystem.h"
#include "VayoLog.h"
#include "VayoUtils.h"

#define TOLUA_REFID_PTR_MAPPING "tolua_refid_ptr_mapping"
#define TOLUA_REFID_TYPE_MAPPING "tolua_refid_type_mapping"
#define TOLUA_REFID_FUNCTION_MAPPING "tolua_refid_function_mapping"

NS_VAYO_BEGIN

lua_State* LuaScriptStack::getLuaState() const
{
	return _state;
}

LuaScriptStack::LuaScriptStack(const wstring& name, LuaScriptSystem* scriptSys)
	: ScriptProtocol(name)
	, _scriptSys(scriptSys)
{
	static unsigned short idx = 0;
	if (0 == _scriptName.compare(L""))
	{
		std::wstringstream ss;
		ss << L"lua_script_" << idx;
		++idx;
		_scriptName = ss.str();
	}

	_state = luaL_newstate();
	luaL_openlibs(_state);

	lua_pushstring(_state, TOLUA_REFID_PTR_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);

	lua_pushstring(_state, TOLUA_REFID_TYPE_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);

	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_newtable(_state);
	lua_rawset(_state, LUA_REGISTRYINDEX);
}

LuaScriptStack::~LuaScriptStack()
{
	lua_close(_state);
}

bool LuaScriptStack::executeString(const string& codes)
{
	int status = lua_status(_state);
	if (status != LUA_OK && status != LUA_ERRRUN)
		return false;

	status = luaL_dostring(_state, codes.c_str());
	lua_gc(_state, LUA_GCCOLLECT, 0);

	if (status != LUA_OK)
	{
		Log::print(ELL_ERROR, "[LuaScriptModule]: %s", lua_tostring(_state, -1));
		lua_pop(_state, 1);
		return false;
	}

	return true;
}

bool LuaScriptStack::executeScriptFile(const wstring& filename)
{
	int status = lua_status(_state);
	if (status != LUA_OK && status != LUA_ERRRUN)
		return false;

	if (filename == L"")
	{
		Log::print(ELL_WARNING, "executeScriptFile filename is null");
		return false;
	}

	bool hasCache = false;
	const char* contentCache = "";
	stringstream filestream;

	std::map<wstring, string>::iterator it = _scriptSys->_scriptFileName2ContentCache.find(filename);
	if (it != _scriptSys->_scriptFileName2ContentCache.end())
	{
		contentCache = it->second.c_str();
		hasCache = true;
	}
	else
	{
		std::ifstream fin(filename);
		if (!fin)
		{
			Log::wprint(ELL_ERROR, L"��[%s]�ļ�ʧ��", filename.c_str());
			return false;
		}

		fin.imbue(locale("chs"));
		filestream << fin.rdbuf();
		contentCache = filestream.str().c_str();
	}

	if (strcmp(contentCache, "") != 0)
	{
		Log::wprint(ELL_ERROR, L"not found ScriptFile: %s", filename.c_str());
		return false;
	}

	status = luaL_loadstring(_state, contentCache);
	lua_gc(_state, LUA_GCCOLLECT, 0);

	if (status != LUA_OK)
	{
		const char* errMsg = lua_tostring(_state, -1);
		Log::print(ELL_ERROR, "[LuaScriptModule]:%s,%s\n", unicodeToUtf8(filename).c_str(), errMsg);
		lua_pop(_state, 1);
		return false;
	}

	status = lua_resume(_state, NULL, 0);
	if (status != LUA_OK)
	{
		const char* errMsg = lua_tostring(_state, -1);
		Log::print(ELL_ERROR, "[LuaScriptModule]:%s,%s\n", unicodeToUtf8(filename).c_str(), errMsg);
		lua_pop(_state, 1);
		return false;
	}

	if (!hasCache)
		_scriptSys->_scriptFileName2ContentCache[filename] = filestream.str();

	return true;
}

int LuaScriptStack::executeGlobalFunction(const string& functionName)
{
	lua_getglobal(_state, functionName.c_str());

	if (!lua_isfunction(_state, -1))
	{
		Log::print(ELL_WARNING, "[LUA ERROR] name '%s' does not represent a Lua function", functionName.c_str());
		lua_pop(_state, 1);
		return 0;
	}

	return executeFunction(0);
}

int LuaScriptStack::executeFunctionByHandler(int nHandler, int numArgs)
{
	int ret = 0;
	if (pushFunctionByHandler(nHandler))                             /* stack: ... arg1 arg2 ... func */
	{
		if (numArgs > 0)
			lua_insert(_state, -(numArgs + 1));                      /* stack: ... func arg1 arg2 ... */
		ret = executeFunction(numArgs);
	}
	lua_settop(_state, 0);
	return ret;
}

void LuaScriptStack::removeScriptHandler(int nHandler)
{
	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(_state, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(_state, nHandler);                               /* stack: ... refid_fun refid */
	lua_pushnil(_state);                                             /* stack: ... refid_fun refid nil */
	lua_rawset(_state, -3);                  /* refid_fun[refid] = nil, stack: ... refid_fun */
	lua_pop(_state, 1);                                              /* stack: ... */
}

int LuaScriptStack::reallocateScriptHandler(int nHandler)
{
	static int s_function_ref_id = 0;
	int nNewHandle = -1;

	if (pushFunctionByHandler(nHandler))
	{
		// function at lo
		int lo = lua_gettop(_state);
		if (!lua_isfunction(_state, lo))
			return nNewHandle;

		++s_function_ref_id;

		lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
		lua_rawget(_state, LUA_REGISTRYINDEX);                       /* stack: fun ... refid_fun */
		lua_pushinteger(_state, s_function_ref_id);                  /* stack: fun ... refid_fun refid */
		lua_pushvalue(_state, lo);                                   /* stack: fun ... refid_fun refid fun */
		lua_rawset(_state, -3);              /* refid_fun[refid] = fun, stack: fun ... refid_fun */
		lua_pop(_state, 1);                                          /* stack: fun ... */

		nNewHandle = s_function_ref_id;
	}

	return nNewHandle;
}

void LuaScriptStack::call_va(const string& func, const string& sig, ...)
{
	const char* pszfunc = func.c_str();
	const char* pszsig = sig.c_str();
	va_list vl;
	int narg, nres; // �����ͽ��������
	va_start(vl, pszsig);
	lua_getglobal(_state, pszfunc); // ѹ�뺯��

	// ѹ�����
	bool endargs = false;
	for (narg = 0; *pszsig; ++narg) // �������в���
	{
		// ���ջ�пռ�
		luaL_checkstack(_state, 1, "too many arguments");

		switch (*pszsig++)
		{
		case 'd': // double����
			lua_pushnumber(_state, va_arg(vl, double));
			break;
		case 'i': // int����
			lua_pushinteger(_state, va_arg(vl, int));
			break;
		case 's': // string����
			lua_pushstring(_state, va_arg(vl, char*));
			break;
		case '>': // ��������
			endargs = true;
			break;
		default:
			Log::print(ELL_ERROR, "invalid option (%c)", *(pszsig - 1));
			lua_error(_state);
			break;
		}

		if (endargs)
			break;
	}

	nres = strlen(pszsig); // �����Ľ������
	if (lua_pcall(_state, narg, nres, 0) != 0) // ��ɵ���
	{
		Log::print(ELL_ERROR, "error calling '%s':%s", pszfunc, lua_tostring(_state, -1));
		lua_error(_state);
	}

	// �����
	nres = -nres;
	while (*pszsig)
	{
		switch (*pszsig++)
		{
		case 'd': // double���
			if (!lua_isnumber(_state, nres))
			{
				Log::print(ELL_ERROR, "error result type");
				lua_error(_state);
			}
			*va_arg(vl, double*) = lua_tonumber(_state, nres);
			break;
		case 'i': // int���
			if (!lua_isnumber(_state, nres))
			{
				Log::print(ELL_ERROR, "error result type");
				lua_error(_state);
			}
			*va_arg(vl, int*) = lua_tointeger(_state, nres);
			break;
		case 's': // string���
			if (!lua_isstring(_state, nres))
			{
				Log::print(ELL_ERROR, "error result type");
				lua_error(_state);
			}
			*va_arg(vl, const char**) = lua_tostring(_state, nres);
			break;
		default:
			Log::print(ELL_ERROR, "invalid option (%c)", *(pszsig - 1));
			lua_error(_state);
			break;
		}

		++nres;
	}

	va_end(vl);
}

int LuaScriptStack::executeFunction(int numArgs)
{
	int functionIndex = -(numArgs + 1);
	if (!lua_isfunction(_state, functionIndex))
	{
		Log::print(ELL_WARNING, "value at stack [%d] is not function", functionIndex);
		lua_pop(_state, numArgs + 1); // remove function and arguments
		return 0;
	}

	int traceback = 0;
	lua_getglobal(_state, "__G__TRACKBACK__");                                 /* stack: ... func arg1 arg2 ... G */

	if (!lua_isfunction(_state, -1))
	{
		lua_pop(_state, 1);                                                    /* stack: ... func arg1 arg2 ... */
	}
	else
	{
		lua_insert(_state, functionIndex - 1);                                 /* stack: ... G func arg1 arg2 ... */
		traceback = functionIndex - 1;
	}

	int error = lua_pcall(_state, numArgs, 1, traceback);                      /* stack: ... [G] ret */
	if (error)
	{
		if (traceback == 0)
		{
			Log::print(ELL_ERROR, "[LUA ERROR] %s", lua_tostring(_state, -1)); /* stack: ... error */
			lua_pop(_state, 1); // remove error message from stack
		}
		else                                                                   /* stack: ... G error */
		{
			lua_pop(_state, 2); // remove __G__TRACKBACK__ and error message from stack
		}

		return 0;
	}

	// get return value
	int ret = 0;
	if (lua_isnumber(_state, -1))
	{
		ret = lua_tointeger(_state, -1);
	}
	else if (lua_isboolean(_state, -1))
	{
		ret = lua_toboolean(_state, -1);
	}

	// remove return value from stack
	lua_pop(_state, 1);                                                        /* stack: ... [G] */

	if (traceback)
		lua_pop(_state, 1); // remove __G__TRACKBACK__ from stack              /* stack: ... */

	return ret;
}

void LuaScriptStack::clean()
{
	lua_settop(_state, 0);
}

void LuaScriptStack::pushInt(int intValue)
{
	lua_pushinteger(_state, intValue);
}

void LuaScriptStack::pushFloat(float floatValue)
{
	lua_pushnumber(_state, floatValue);
}

void LuaScriptStack::pushBoolean(bool boolValue)
{
	lua_pushboolean(_state, boolValue);
}

void LuaScriptStack::pushString(const char* stringValue)
{
	lua_pushstring(_state, stringValue);
}

void LuaScriptStack::pushString(const char* stringValue, int length)
{
	lua_pushlstring(_state, stringValue, length);
}

void LuaScriptStack::pushNil(void)
{
	lua_pushnil(_state);
}

bool LuaScriptStack::pushFunctionByHandler(int nHandler)
{
	lua_pushstring(_state, TOLUA_REFID_FUNCTION_MAPPING);
	lua_rawget(_state, LUA_REGISTRYINDEX);                           /* stack: ... refid_fun */
	lua_pushinteger(_state, nHandler);                               /* stack: ... refid_fun refid */
	lua_rawget(_state, -2);                                          /* stack: ... refid_fun fun */
	lua_remove(_state, -2);                                          /* stack: ... fun */

	if (!lua_isfunction(_state, -1))
	{
		Log::print(ELL_ERROR, "[LUA ERROR] function refid '%d' does not reference a Lua function", nHandler);
		lua_pop(_state, 1);
		return false;
	}

	return true;
}

NS_VAYO_END
