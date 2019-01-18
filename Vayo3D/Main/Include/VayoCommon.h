/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 公共宏和枚举值定义
\*************************************************************************/
#ifndef __VAYO_COMMON_H__
#define __VAYO_COMMON_H__

//=========================================================================
// 宏定义
//=========================================================================
#define NS_VAYO_BEGIN namespace Vayo {
#define NS_VAYO_END }

#define VAYO_DISABLE_ASSERT 0
#if VAYO_DISABLE_ASSERT > 0
#define VAYO_ASSERT(cond)
#else
#include <cassert>
#define VAYO_ASSERT(cond)    assert(cond)
#endif

#if defined(_DEBUG) && defined(VAYO_ENABLE_MEM_LEAK_CHECK)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
// Enable run-time memory check for debug builds.
#define EnableMemLeakCheck() _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF)
#else
#define EnableMemLeakCheck()
#endif

#if defined(VAYO_STATIC_LIB)
#	define _VayoExport
#else
#	if defined(VAYO_NONCLIENT_BUILD)
#		define _VayoExport __declspec(dllexport)
#	else
#		if defined(__MINGW32__)
#			define _VayoExport
#		else
#			define _VayoExport __declspec(dllimport)
#		endif
#	endif
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL    0
#else
#define NULL    ((void *)0)
#endif
#endif

#define SAFE_DELETE(p)       do { if (p) { delete (p); (p) = 0; } } while (0)
#define SAFE_DELETE_ARRAY(p) do { if (p) { delete [] (p); (p) = 0; } } while (0)
#define IF_FALSE_BREAK(p)    if (!(p)) break
#define MAX(x, y)            (((x)>(y)) ? (x) : (y))
#define MIN(x, y)            (((x)<(y)) ? (x) : (y))

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                 \
private: TypeName(const TypeName&);                                        \
private: TypeName& operator=(const TypeName&);

#define DISALLOW_COPY(TypeName)                                            \
private: TypeName(const TypeName&);

#define DISALLOW_ASSIGN(TypeName)                                          \
private: TypeName& operator=(const TypeName&);

#define PROPERTY_R(varType, varName, funName)                              \
protected: varType varName;                                                \
public: varType get##funName(void) const { return varName; }

#define PROPERTY_VR(varType, varName, funName)                             \
protected: varType varName;                                                \
public: virtual varType get##funName(void) const { return varName; }

#define PROPERTY_R_REF(varType, varName, funName)                          \
protected: varType varName;                                                \
public: const varType& get##funName(void) const { return varName; }

#define PROPERTY_VR_REF(varType, varName, funName)                         \
protected: varType varName;                                                \
public: virtual const varType& get##funName(void) const { return varName; }

#define PROPERTY_RW(varType, varName, funName)                             \
protected: varType varName;                                                \
public: varType get##funName(void) const { return varName; }               \
public: void set##funName(varType var) { varName = var; }

#define PROPERTY_VRW(varType, varName, funName)                            \
protected: varType varName;                                                \
public: virtual varType get##funName(void) const { return varName; }       \
public: virtual void set##funName(varType var) { varName = var; }

#define PROPERTY_RW_REF(varType, varName, funName)                         \
protected: varType varName;                                                \
public: const varType& get##funName(void) const { return varName; }        \
public: void set##funName(const varType& var) { varName = var; }

#define PROPERTY_VRW_REF(varType, varName, funName)                        \
protected: varType varName;                                                \
public: virtual const varType& get##funName(void) const { return varName; }\
public: virtual void set##funName(const varType& var) { varName = var; }

//=========================================================================
// 枚举类型
//=========================================================================
NS_VAYO_BEGIN

enum ERenderSystemType
{
	ERST_NULL,
	ERST_D3D9,
	ERST_D3D11,
	ERST_OPENGL
};

enum EPrimitiveType
{
	EPT_POINTS,
	EPT_LINES,
	EPT_LINE_STRIP,
	EPT_LINE_LOOP,
	EPT_TRIANGLES,
	EPT_TRIANGLE_STRIP,
	EPT_TRIANGLE_FAN,
	EPT_QUADS,
	EPT_QUAD_STRIP,
	EPT_POLYGON
};

enum EHardwareMapping
{
	EHM_NEVER = 0,
	EHM_STATIC,
	EHM_DYNAMIC,
	EHM_STREAM
};

enum EBufferType
{
	EBT_NONE = 0,
	EBT_VERTEX,
	EBT_INDEX,
	EBT_VERTEX_AND_INDEX
};

NS_VAYO_END

#endif // __VAYO_COMMON_H__
