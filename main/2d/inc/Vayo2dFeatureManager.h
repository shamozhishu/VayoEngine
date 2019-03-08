/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 特征管理器
\*************************************************************************/
#ifndef __VAYO2D_FEATURE_MANAGER_H__
#define __VAYO2D_FEATURE_MANAGER_H__

/* ----------begin----------特征脚本----------
feature examples/test
{
	fill on
	color 1 1 1 1
	stroke_width 1.0
	picture test0.png
	stroke_style
	{
		start_cap flat
		end_cap square
		dash_cap round
		line_join line_round
		miter_limit 10.0
		dash_style solid
		dash_offset 0.0
	}
}
** ----------end------------特征脚本----------
*/

#include "Vayo2dSupport.h"
#include "Vayo2dFeature.h"
NS_VAYO2D_BEGIN

typedef struct tagFeatureScriptAttrib
{
	const char* TheKey;
	int         TheValue;
} FeatureScriptAttrib;

const FeatureScriptAttrib featureScriptAttribs[] =
{
	// cap style
	{ "flat", ECS_FLAT },
	{ "square", ECS_SQUARE },
	{ "round", ECS_ROUND },
	{ "triangle", ECS_TRIANGLE },
	// dash style
	{ "solid", EDS_SOLID },
	{ "dash", EDS_DASH },
	{ "dot", EDS_DOT },
	{ "dash_dot", EDS_DASH_DOT },
	{ "dash_dot_dot", EDS_DASH_DOT_DOT },
	{ "custom", EDS_CUSTOM },
	// line join
	{ "miter", ELJ_MITER },
	{ "bevel", ELJ_BEVEL },
	{ "line_round", ELJ_ROUND },
	{ "miter_or_bevel", ELJ_MITER_OR_BEVEL },
	// end mark
	{ nullptr, 0 }
};

class _Vayo2dExport FeatureManager
{
	DISALLOW_COPY_AND_ASSIGN(FeatureManager)
public:
	FeatureManager();
	~FeatureManager();
	bool       init();
	bool       parseFeature(const wstring& filename, bool fullPath = false);
	bool       parseFeature(stringstream& filestream);
	FeaturePtr createFeature(const wstring& name = L"");
	FeaturePtr findFeature(const wstring& name);
	void       destroyFeature(FeaturePtr feature);
	void       destroyFeature(const wstring& name);
	void       clearAllFeatures();
	FeaturePtr getDefaultFeature() const;

private:
	FeaturePtr                 _defaultFeature;
	map<wstring, FeaturePtr>   _featurePool;
	unordered_map<string, int> _attribsWordMap;
};

NS_VAYO2D_END

#endif // __VAYO2D_FEATURE_MANAGER_H__
