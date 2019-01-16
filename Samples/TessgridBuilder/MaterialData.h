#pragma once

class CMaterialData
{
public:
	CMaterialData();
	void reset();
public:
	CString name;
	CString type;
	unsigned char ambient_color[4];
	unsigned char diffuse_color[4];
	unsigned char specular_color[4];
	unsigned char emissive_color[4];
	float shininess;
	float thickness;
	float alpha_ref;
	CString anti_aliasing;
	unsigned char stencil_mask;
	unsigned char stencil_func_mask;
	int stencil_ref;
	CString stencil_func;
	CString stencil_fail;
	CString depth_fail;
	CString stencil_depth_pass;
	CString wireframe;
	CString clockwise;
	CString gouraud_shading;
	CString lighting;
	CString zbuffer;
	CString zwrite_enable;
	CString stencil_buffer;
	CString backface_culling;
	CString use_mip_maps;
	CString fog_enable;
	CString vertex_program;
	CString fragment_program;
	unsigned short shader_constant_set_callback;

	struct STextureUnit
	{
		CString texture;
		CString filtering;
		CString tex_address_mode;
	} texture_unit[MATERIAL_MAX_TEXTURES];
};
