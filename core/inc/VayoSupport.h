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

namespace Vayo
{
	class Language;
	class DatabaseCSV;
	class ScriptSystem;
	class ConfigManager;
	class Plugin;
	class DynLib;
	class DynLibManager;
	class TouchDispatcher;
	class KeypadDispatcher;

	template<typename T> class SharedPtr;
	template<typename ...Args> class ReflexFactory;
}

#endif // __VAYO_SUPPORT_H__
