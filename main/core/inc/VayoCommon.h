/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 公共宏定义
\*************************************************************************/
#ifndef __VAYO_COMMON_H__
#define __VAYO_COMMON_H__

#define NS_VAYO_BEGIN namespace Vayo {
#define NS_VAYO_END }

#define NS_VAYO2D_BEGIN namespace Vayo { namespace _2d {
#define NS_VAYO2D_END }}

#define NS_VAYO3D_BEGIN namespace Vayo { namespace _3d {
#define NS_VAYO3D_END }}

#define VAYO_DISABLE_ASSERT 0
#if VAYO_DISABLE_ASSERT > 0
#define VAYO_ASSERT(cond)
#else
#include <cassert>
#define VAYO_ASSERT(cond)    assert(cond)
#endif

#if defined(_DEBUG) || defined(VAYO_ENABLE_LOG_PRINT)
#define VayoLogPrint(FILENAME) static Vayo::Log s_logger(true, #FILENAME);
#define VayoLogPrintFile(FILENAME) static Vayo::Log s_logger(false, #FILENAME);
#define VayoLogPrintConsole() static Vayo::Log s_logger;
#else
#define VayoLogPrint(FILENAME)
#define VayoLogPrintFile(FILENAME)
#define VayoLogPrintConsole()
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

#if defined(VAYO2D_STATIC_LIB)
#	define _Vayo2dExport
#else
#	if defined(VAYO2D_NONCLIENT_BUILD)
#		define _Vayo2dExport __declspec(dllexport)
#	else
#		if defined(__MINGW32__)
#			define _Vayo2dExport
#		else
#			define _Vayo2dExport __declspec(dllimport)
#		endif
#	endif
#endif

#if defined(VAYO3D_STATIC_LIB)
#	define _Vayo3dExport
#else
#	if defined(VAYO3D_NONCLIENT_BUILD)
#		define _Vayo3dExport __declspec(dllexport)
#	else
#		if defined(__MINGW32__)
#			define _Vayo3dExport
#		else
#			define _Vayo3dExport __declspec(dllimport)
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

#endif // __VAYO_COMMON_H__
