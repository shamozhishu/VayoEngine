#include "stdafx.h"
#include "MaterialData.h"

CMaterialData::CMaterialData()
	: name(_T(""))
	, type(_T("solid"))
	, shininess(0.0f)
	, thickness(1.0f)
	, alpha_ref(0.0f)
	, anti_aliasing(_T("on"))
	, stencil_mask(255)
	, stencil_func_mask(255)
	, stencil_ref(0)
	, stencil_func(_T("never"))
	, stencil_fail(_T("keep"))
	, depth_fail(_T("keep"))
	, stencil_depth_pass(_T("keep"))
	, wireframe(_T("off"))
	, clockwise(_T("off"))
	, gouraud_shading(_T("on"))
	, lighting(_T("on"))
	, zbuffer(_T("on"))
	, zwrite_enable(_T("on"))
	, stencil_buffer(_T("off"))
	, backface_culling(_T("on"))
	, use_mip_maps(_T("on"))
	, fog_enable(_T("off"))
	, vertex_program(_T(""))
	, fragment_program(_T(""))
	, shader_constant_set_callback(0)
{
	ambient_color[0] = ambient_color[1] = ambient_color[2] = ambient_color[3] = 255;
	diffuse_color[0] = diffuse_color[1] = diffuse_color[2] = diffuse_color[3] = 255;
	specular_color[0] = specular_color[1] = specular_color[2] = specular_color[3] = 0;
	emissive_color[0] = emissive_color[1] = emissive_color[2] = emissive_color[3] = 0;

	for (int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		texture_unit[i].texture.Empty();
		texture_unit[i].filtering = _T("bilinear_filter");
		texture_unit[i].tex_address_mode = _T("repeat");
	}
}

void CMaterialData::reset()
{
	name.Empty();
	type = _T("solid");
	shininess = 0.0f;
	thickness = 1.0f;
	alpha_ref = 0.0f;
	anti_aliasing = _T("on");
	stencil_mask = 255;
	stencil_func_mask = 255;
	stencil_ref = 0;
	stencil_func = _T("never");
	stencil_fail = _T("keep");
	depth_fail = _T("keep");
	stencil_depth_pass = _T("keep");
	wireframe = _T("off");
	clockwise = _T("off");
	gouraud_shading = _T("on");
	lighting = _T("on");
	zbuffer = _T("on");
	zwrite_enable = _T("on");
	stencil_buffer = _T("off");
	backface_culling = _T("on");
	use_mip_maps = _T("on");
	fog_enable = _T("off");
	vertex_program.Empty();
	fragment_program.Empty();
	shader_constant_set_callback = 0;

	ambient_color[0] = ambient_color[1] = ambient_color[2] = ambient_color[3] = 255;
	diffuse_color[0] = diffuse_color[1] = diffuse_color[2] = diffuse_color[3] = 255;
	specular_color[0] = specular_color[1] = specular_color[2] = specular_color[3] = 0;
	emissive_color[0] = emissive_color[1] = emissive_color[2] = emissive_color[3] = 0;

	for (int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
	{
		texture_unit[i].texture.Empty();
		texture_unit[i].filtering = _T("bilinear_filter");
		texture_unit[i].tex_address_mode = _T("repeat");
	}
}
