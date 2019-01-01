/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 材质管理器
\*************************************************************************/
#ifndef __VAYO_MATERIAL_MANAGER_H__
#define __VAYO_MATERIAL_MANAGER_H__

/* ----------begin----------材质脚本----------
material examples/test
{
	type solid
	ambient_color 104 204 50 255
	diffuse_color 153 153 153 255
	specular_color 102 102 102 255
	emissive_color 0 0 0 0
	shininess 16
	//thickness 1
	//alpha_ref 0
	//anti_aliasing line_smooth
	stencil_mask 255
	stencil_func_mask 255
	stencil_ref 1
	stencil_func always
	stencil_fail keep
	depth_fail keep
	stencil_depth_pass replace
	wireframe off
	clockwise off
	gouraud_shading on
	lighting on
	zbuffer on
	zwrite_enable on
	stencil_buffer on
	backface_culling on
	use_mip_maps on
	fog_enable off
	vertex_program GLSL330/multiple_lights.vs
	fragment_program GLSL330/multiple_lights.fs
	shader_constant_set_callback 0

	texture_unit 0
	{
		texture test0.png
		filtering nearest_filter
		tex_address_mode texture_clamp_repeat
	}

	texture_unit 1
	{
		texture test1.jpg
		filtering bilinear_filter
		tex_address_mode texture_clamp_clamp
	}

	texture_unit 2
	{
		texture test2.jpg
		filtering trilinear_filter
		tex_address_mode texture_clamp_mirror
	}
}
** ----------end----------材质脚本----------
*/

#include "VayoSupport.h"
#include "VayoMaterial.h"
#include "VayoMaterialRenderer.h"

#define VAYO_MAX_SHADER_CALLBACK_NUM (500)

NS_VAYO_BEGIN

typedef struct tagMaterialScriptAttrib
{
	const char* TheKey;
	int         TheValue;
} MaterialScriptAttrib;

const MaterialScriptAttrib materialScriptAttribs[] =
{
	// texture filter
	{ "nearest_filter", 1 },
	{ "bilinear_filter", 2 },
	{ "trilinear_filter", 3 },
	// stencil func
	{ "never", ESF_NEVER },
	{ "less", ESF_LESS },
	{ "lequal", ESF_LEQUAL },
	{ "greater", ESF_GREATER },
	{ "gequal", ESF_GEQUAL },
	{ "equal", ESF_EQUAL },
	{ "notequal", ESF_NOTEQUAL },
	{ "always", ESF_ALWAYS },
	// stencil op
	{ "keep", ESO_KEEP },
	{ "zero", ESO_ZERO },
	{ "replace", ESO_REPLACE },
	{ "incr", ESO_INCR },
	{ "oncr_wrap", ESO_INCR_WRAP },
	{ "decr", ESO_DECR },
	{ "decr_wrap", ESO_DECR_WRAP },
	{ "invert", ESO_INVERT },
	// anti aliasing mode
	{ "off", EAAM_OFF },
	{ "on", EAAM_ON },
	{ "line_smooth", EAAM_LINE_SMOOTH },
	{ "point_smooth", EAAM_POINT_SMOOTH },
	{ "alpha_to_coverage", EAAM_ALPHA_TO_COVERAGE },
	// material type
	{ "solid", EMT_SOLID },
	{ "solid_2_layer", EMT_SOLID_2_LAYER },
	{ "lightmap", EMT_LIGHTMAP },
	{ "lightmap_add", EMT_LIGHTMAP_ADD },
	{ "lightmap_m2", EMT_LIGHTMAP_M2 },
	{ "lightmap_m4", EMT_LIGHTMAP_M4 },
	{ "lightmap_lighting", EMT_LIGHTMAP_LIGHTING },
	{ "lightmap_lighting_m2", EMT_LIGHTMAP_LIGHTING_M2 },
	{ "lightmap_lighting_m4", EMT_LIGHTMAP_LIGHTING_M4 },
	{ "sphere_map", EMT_SPHERE_MAP },
	{ "reflection_2_layer", EMT_REFLECTION_2_LAYER },
	{ "transparent_add_color", EMT_TRANSPARENT_ADD_COLOR },
	{ "transparent_alpha_channel", EMT_TRANSPARENT_ALPHA_CHANNEL },
	{ "transparent_vertex_alpha", EMT_TRANSPARENT_VERTEX_ALPHA },
	{ "transparent_reflection_2_layer", EMT_TRANSPARENT_REFLECTION_2_LAYER },
	// end mark
	{ NULL, 0 }
};

class _VayoExport MaterialManager
{
	DISALLOW_COPY_AND_ASSIGN(MaterialManager)
public:
	MaterialManager();
	~MaterialManager();
	bool init();
	MaterialPtr createMaterial(const wstring& name = L"");
	MaterialPtr findMaterial(const wstring& name);
	void        destroyMaterial(const wstring& name);
	void        destroyMaterial(const MaterialPtr& ptr);
	MaterialPtr getDefaultMaterial() const;
	void registerCallback(unsigned int idx, ShaderConstantSetCallback callback);
	void unregisterCallback(unsigned int idx);

private:
	bool parseMaterial(const wstring& filePath);

private:
	MaterialPtr                _defaultMaterial;
	map<wstring, MaterialPtr>  _materialPool;
	unordered_map<string, int> _attribsWordMap;
	ShaderConstantSetCallback  _materialCallback[VAYO_MAX_SHADER_CALLBACK_NUM];
};

NS_VAYO_END

#endif // __VAYO_MATERIAL_MANAGER_H__
