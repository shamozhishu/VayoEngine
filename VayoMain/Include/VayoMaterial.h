/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 材质
\*************************************************************************/
#ifndef __VAYO_MATERIAL_H__
#define __VAYO_MATERIAL_H__

#include "VayoColour.h"
#include "VayoTextureLayer.h"
NS_VAYO_BEGIN

enum EMaterialType
{
	EMT_SOLID = 0,

	// Solid material with 2 texture layers. The second is blended onto the
	// first using the alpha value of the vertex colors.
	// This material is currently not implemented in OpenGL, but it
	// works with DirectX.
	EMT_SOLID_2_LAYER,

	// Material type with standard lightmap technique: 
	// There should be 2 textures: The first texture layer is a diffuse map,
	// the second is a light map. Vertex light is ignored.
	EMT_LIGHTMAP,

	// Material type with lightmap technique like EMT_LIGHTMAP, but
	// lightmap and diffuse texture are not modulated, but added instead.
	EMT_LIGHTMAP_ADD,

	// Material type with standard lightmap technique: 
	// There should be 2 textures: The first texture layer is a diffuse map,
	// the second is a light map. Vertex light is ignored.
	// The texture colors are effectively multiplyied by 2 for brightening.
	// like known in DirectX as D3DTOP_MODULATE2X.
	EMT_LIGHTMAP_M2,

	// Material type with standard lightmap technique: 
	// There should be 2 textures: The first texture layer is a diffuse map,
	// the second is a light map. Vertex light is ignored.
	// The texture colors are effectively multiplyied by 4 for brightening.
	// like known in DirectX as D3DTOP_MODULATE4X.
	EMT_LIGHTMAP_M4,

	// Like EMT_LIGHTMAP, but also supports dynamic lighting.
	EMT_LIGHTMAP_LIGHTING,

	// Like EMT_LIGHTMAP_M2, but also supports dynamic lighting.
	EMT_LIGHTMAP_LIGHTING_M2,

	// Like EMT_LIGHTMAP_4, but also supports dynamic lighting.
	EMT_LIGHTMAP_LIGHTING_M4,

	// Makes the material look like it was reflection the environment
	// around it. To make this possible, a texture called 'sphere map'
	// is used, which must be set as Texture1.
	EMT_SPHERE_MAP,

	// A reflecting material with an 
	// optional additional non reflecting texture layer. The reflection
	// map should be set as Texture 1.
	// Please note that this material type is currently not 100% implemented
	// in OpenGL. It works in DirectX8.
	EMT_REFLECTION_2_LAYER,

	// A transparent material. Only the first texture is used.
	// The new color is calculated by simply adding the source color and
	// the dest color. This means if for example a billboard using a texture with 
	// black background and a red circle on it is drawed with this material, the
	// result is that only the red circle will be drawn a little bit transparent,
	// and everything which was black is 100% transparent and not visible.
	// This material type is useful for e.g. particle effects.
	EMT_TRANSPARENT_ADD_COLOR,

	// Makes the material transparent based on the texture alpha channel.
	// Only first texture is used. Please note that this material is currenly
	// NOT IMPLEMENTED.
	EMT_TRANSPARENT_ALPHA_CHANNEL,

	// Makes the material transparent based on the vertex alpha value.
	EMT_TRANSPARENT_VERTEX_ALPHA,

	// A transparent reflecting material with an 
	// optional additional non reflecting texture layer. The reflection
	// map should be set as Texture 1. The transparency depends on the
	// alpha value in the vertex colors. A texture which will not reflect
	// can be set als Texture 2.
	// Please note that this material type is currently not 100% implemented
	// in OpenGL. It works in DirectX8.
	EMT_TRANSPARENT_REFLECTION_2_LAYER,

	// This value is not used. It only forces this enumeration to compile in 32 bit. 
	EMT_FORCE_32BIT = 0x7FFFFFFF
};

enum EMaterialFlag
{
	EMF_WIREFRAME = 0,
	EWF_CLOCKWISE,
	EMF_GOURAUD_SHADING,
	EMF_LIGHTING,
	EMF_ZBUFFER,
	EMF_ZWRITE_ENABLE,
	EMF_STENCIL_BUFFER,
	EMF_BACK_FACE_CULLING,
	EMF_USE_MIPMAPS,
	EMF_FOG_ENABLE,
	EMF_MATERIAL_FLAG_COUNT
};

enum EAntiAliasingMode
{
	EAAM_OFF = 0,
	EAAM_ON = 1,
	EAAM_LINE_SMOOTH = 4,
	EAAM_POINT_SMOOTH = 8,
	EAAM_ALPHA_TO_COVERAGE = 16
};

enum EStencilFunc
{
	ESF_NEVER,
	ESF_LESS,
	ESF_LEQUAL,
	ESF_GREATER,
	ESF_GEQUAL,
	ESF_EQUAL,
	ESF_NOTEQUAL,
	ESF_ALWAYS
};

enum EStencilOp
{
	ESO_KEEP,
	ESO_ZERO,
	ESO_REPLACE,
	ESO_INCR,
	ESO_INCR_WRAP,
	ESO_DECR,
	ESO_DECR_WRAP,
	ESO_INVERT
};

const int MATERIAL_MAX_TEXTURES = VAYO_MATERIAL_MAX_TEXTURES;

class _VayoExport Material
{
public:
	Material()
		: _materialName(L"")
		, _materialType(EMT_SOLID)
		, _ambientColor(255, 255, 255, 255)
		, _diffuseColor(255, 255, 255, 255)
		, _specularColor(0, 0, 0, 0)
		, _emissiveColor(0, 0, 0, 0)
		, _shininess(0.0f)
		, _thickness(1.0f)
		, _alphaRef(0.0f)
		, _antiAliasing(EAAM_ON)
		, _stencilMask(0xFF)
		, _stencilFuncMask(0xFF)
		, _stencilRef(0)
		, _stencilFunc(ESF_NEVER)
		, _stencilFail(ESO_KEEP)
		, _depthFail(ESO_KEEP)
		, _stencilDepthPass(ESO_KEEP)
		, _vertexSource(L"")
		, _fragmentSource(L"")
		, _shaderCallback(0)
		, _wireframe(false)
		, _clockwise(false)
		, _gouraudShading(true)
		, _lighting(true)
		, _zBuffer(true)
		, _zWriteEnable(true)
		, _stencilBuffer(false)
		, _backfaceCulling(true)
		, _useMipMaps(true)
		, _fogEnable(false) {}

	Material(const Material& other)
	{
		*this = other;
	}

	Material& operator=(const Material& other)
	{
		if (this == &other)
			return *this;

		_materialName = other._materialName;
		_materialType = other._materialType;
		_ambientColor = other._ambientColor;
		_diffuseColor = other._diffuseColor;
		_specularColor = other._specularColor;
		_emissiveColor = other._emissiveColor;
		_shininess = other._shininess;
		_thickness = other._thickness;
		_alphaRef = other._alphaRef;
		_antiAliasing = other._antiAliasing;
		_stencilMask = other._stencilMask;
		_stencilFuncMask = other._stencilFuncMask;
		_stencilRef = other._stencilRef;
		_stencilFunc = other._stencilFunc;
		_stencilFail = other._stencilFail;
		_depthFail = other._depthFail;
		_stencilDepthPass = other._stencilDepthPass;
		_vertexSource = other._vertexSource;
		_fragmentSource = other._fragmentSource;
		_shaderCallback = other._shaderCallback;

		_wireframe = other._wireframe;
		_clockwise = other._clockwise;
		_gouraudShading = other._gouraudShading;
		_lighting = other._lighting;
		_zBuffer = other._zBuffer;
		_zWriteEnable = other._zWriteEnable;
		_stencilBuffer = other._stencilBuffer;
		_backfaceCulling = other._backfaceCulling;
		_useMipMaps = other._useMipMaps;
		_fogEnable = other._fogEnable;

		for (unsigned int i = 0; i < MATERIAL_MAX_TEXTURES; ++i)
		{
			_textureLayer[i] = other._textureLayer[i];
		}

		return *this;
	}

	inline bool operator!=(const Material& b) const
	{
		return _materialType != b._materialType ||
			_ambientColor != b._ambientColor ||
			_diffuseColor != b._diffuseColor ||
			_specularColor != b._specularColor ||
			_emissiveColor != b._emissiveColor ||
			_shininess != b._shininess ||
			_thickness != b._thickness ||
			_alphaRef != b._alphaRef ||
			_antiAliasing != b._antiAliasing ||
			_stencilMask != b._stencilMask ||
			_stencilFuncMask != b._stencilFuncMask ||
			_stencilRef != b._stencilRef ||
			_stencilFunc != b._stencilFunc ||
			_stencilFail != b._stencilFail ||
			_depthFail != b._depthFail ||
			_stencilDepthPass != b._stencilDepthPass ||
			_wireframe != b._wireframe ||
			_clockwise != _clockwise ||
			_gouraudShading != b._gouraudShading ||
			_lighting != b._lighting ||
			_zBuffer != b._zBuffer ||
			_zWriteEnable != b._zWriteEnable ||
			_stencilBuffer != b._stencilBuffer ||
			_backfaceCulling != b._backfaceCulling ||
			_useMipMaps != b._useMipMaps ||
			_fogEnable != b._fogEnable;
	}

	inline bool operator==(const Material& b) const
	{
		return !(b != *this);
	}

public:
	wstring        _materialName;
	EMaterialType  _materialType;
	Colour         _ambientColor;
	Colour         _diffuseColor;
	Colour         _specularColor;
	Colour         _emissiveColor;
	float          _shininess;
	float          _thickness;
	float          _alphaRef;
	unsigned char  _antiAliasing;
	unsigned char  _stencilMask;
	unsigned char  _stencilFuncMask;
	int            _stencilRef;
	EStencilFunc   _stencilFunc;
	EStencilOp     _stencilFail;
	EStencilOp     _depthFail;
	EStencilOp     _stencilDepthPass;
	wstring        _vertexSource;
	wstring        _fragmentSource;
	unsigned int   _shaderCallback;
	TextureLayer   _textureLayer[MATERIAL_MAX_TEXTURES];

	union
	{
		struct
		{
			bool _wireframe;
			bool _clockwise;
			bool _gouraudShading;
			bool _lighting;
			bool _zBuffer;
			bool _zWriteEnable;
			bool _stencilBuffer;
			bool _backfaceCulling;
			bool _useMipMaps;
			bool _fogEnable;
		};
		bool _flags[EMF_MATERIAL_FLAG_COUNT];
	};
};

NS_VAYO_END

#endif // __VAYO_MATERIAL_H__
