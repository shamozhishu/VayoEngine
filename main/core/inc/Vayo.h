/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 提供给用户使用的头文件
\*************************************************************************/
#ifndef __VAYO_H__
#define __VAYO_H__

// 引擎核心
#include "VayoCore.h"
// 公共宏定义
#include "VayoCommon.h"
// 第三方支持
#include "VayoSupport.h"
// 输入事件
#include "VayoInput.h"
// 插件模块
#include "VayoDynLib.h"
#include "VayoDynLibManager.h"
#include "VayoPlugin.h"
#include "VayoPluginSystem.h"
// 客户端网络模块
#include "VayoSocket.h"
// 其他主要模块
#include "VayoLanguage.h"
#include "VayoDatabaseCSV.h"
#include "VayoConfigManager.h"
#include "VayoScriptSystem.h"
// 工具类/函数
#include "VayoAny.h"
#include "VayoLog.h"
#include "VayoUtils.h"
#include "VayoTimer.h"
#include "VayoTable.h"
#include "VayoFileIO.h"
#include "VayoSigslot.h"
#include "VayoBitState.h"
#include "VayoSharedPtr.h"
#include "VayoSingleton.h"
#include "VayoReflection.h"
#include "VayoUserDataBind.h"
#include "VayoFrameListener.h"
#include "VayoAttribSerializer.h"
// 数学库
#include "VayoMath.h"
#include "VayoVector2d.h"
#include "VayoLine2d.h"
#include "VayoDimension2d.h"
#include "VayoRectangle.h"
#include "VayoVector3d.h"
#include "VayoLine3d.h"
#include "VayoPlane3d.h"
#include "VayoAabbox3d.h"
#include "VayoTriangle3d.h"
#include "VayoMatrix4x4.h"
#include "VayoQuaternion.h"
// 应用程序基类
#include "VayoApp.h"

#endif // __VAYO_H__
