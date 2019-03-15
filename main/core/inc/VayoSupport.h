/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 第三方支持
\*************************************************************************/
#ifndef __VAYO_SUPPORT_H__
#define __VAYO_SUPPORT_H__

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <cstdarg>
#include <cmath>
// STL containers
#include <vector>
#include <map>
#include <string>
#include <set>
#include <list>
#include <queue>
#include <deque>
#include <stack>
#include <bitset>
#include <unordered_map>
#include <unordered_set>
// STL algorithms & functions
#include <algorithm>
#include <functional>
#include <limits>
#include <typeinfo>
#include <memory>
// C++ Stream stuff
#include <fstream>
#include <iostream>
#include <iomanip>
#include <sstream>

extern "C"
{
#include <sys/types.h>
#include <sys/stat.h>
}

#include "VayoCommon.h"
using namespace std;

NS_VAYO_BEGIN

enum EDeviceID
{
	EDID_AUX_DEVICE0 = 0,
	EDID_AUX_DEVICE1,
	EDID_AUX_DEVICE2,
	EDID_AUX_DEVICE3,
	EDID_AUX_DEVICE4,
	EDID_AUX_DEVICE5,
	EDID_AUX_DEVICE6,
	EDID_AUX_DEVICE7,
	EDID_MAIN_DEVICE,
	EDID_DEVICE_COUNT
};

class App;
class Language;
class DatabaseCSV;
class ScriptSystem;
class ConfigManager;
class Device;
class Plugin;
class DynLib;
class DynLibManager;
class FrameListener;
class TouchDispatcher;
class KeypadDispatcher;

template<typename T> class SharedPtr;
template<typename ...Args> class ReflexFactory;

NS_VAYO_END

#endif // __VAYO_SUPPORT_H__
