/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 纹理层
\*************************************************************************/
#ifndef __VAYO3D_TEXTURE_LAYER_H__
#define __VAYO3D_TEXTURE_LAYER_H__

#define VAYO_MATERIAL_MAX_TEXTURES 4

#include "Vayo3dSupport.h"
#include "VayoSharedPtr.h"
#include "math/VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

enum ETextureClamp
{
	ETC_REPEAT = 0,
	ETC_CLAMP,
	ETC_CLAMP_TO_EDGE,
	ETC_CLAMP_TO_BORDER,
	ETC_MIRROR,
	ETC_MIRROR_CLAMP,
	ETC_MIRROR_CLAMP_TO_EDGE,
	ETC_MIRROR_CLAMP_TO_BORDER
};

static const char* const aTextureClampNames[] = {
	"repeat",
	"clamp",
	"clamp_to_edge",
	"clamp_to_border",
	"mirror",
	"mirror_clamp",
	"mirror_clamp_to_edge",
	"mirror_clamp_to_border", NULL };

class TextureLayer
{
public:
	TextureLayer()
		: _texture(NULL)
		, _textureWrapU(ETC_REPEAT)
		, _textureWrapV(ETC_REPEAT)
		, _bilinearFilter(true)
		, _trilinearFilter(false)
		, _lodBias(0) {}

	TextureLayer(const TextureLayer& other)
	{
		*this = other;
	}

	TextureLayer& operator=(const TextureLayer& other)
	{
		if (this == &other)
			return *this;

		_texture = other._texture;
		_textureWrapU = other._textureWrapU;
		_textureWrapV = other._textureWrapV;
		_bilinearFilter = other._bilinearFilter;
		_trilinearFilter = other._trilinearFilter;
		_lodBias = other._lodBias;
		_textureMatrix = other._textureMatrix;

		return *this;
	}

	inline bool operator!=(const TextureLayer& b) const
	{
		bool different =
			_texture != b._texture ||
			_textureWrapU != b._textureWrapU ||
			_textureWrapV != b._textureWrapV ||
			_bilinearFilter != b._bilinearFilter ||
			_trilinearFilter != b._trilinearFilter ||
			_lodBias != b._lodBias;

		if (different)
			return true;
		else
			different |= (_textureMatrix != b._textureMatrix);

		return different;
	}

	inline bool operator==(const TextureLayer& b) const
	{
		return !(b != *this);
	}

	TexturePtr    _texture;
	unsigned char _textureWrapU : 4;
	unsigned char _textureWrapV : 4;
	bool          _bilinearFilter : 1;
	bool          _trilinearFilter : 1;
	char          _lodBias;
	Matrix4x4     _textureMatrix;
};

NS_VAYO3D_END

#endif // __VAYO3D_TEXTURE_LAYER_H__
