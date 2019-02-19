/*************************************************************************\
* Õ˚”—“˝«Ê1.0
* Copyright (c) 2018-2019 authored by ÷Ïº”∫∆
* ≤ƒ÷ ‰÷»æ∆˜
\*************************************************************************/
#ifndef __VAYO3D_MATERIAL_RENDERER_H__
#define __VAYO3D_MATERIAL_RENDERER_H__

#include "Vayo3dSupport.h"
#include "Vayo3dMaterial.h"
#include "math/VayoVector2d.h"
#include "math/VayoVector3d.h"
#include "math/VayoMatrix4x4.h"
NS_VAYO3D_BEGIN

typedef void(*ShaderConstantSetCallback)(MaterialRendererServices* services);

class MaterialRenderer
{
public:
	virtual ~MaterialRenderer() {}
	virtual void onSetMaterial(Material& material, const Material& lastMaterial, bool resetAllRenderstates) {}
	virtual void onUnsetMaterial() {}
	virtual bool isTransparent() const { return false; }
};

class MaterialRendererServices
{
public:
	virtual void setShaderConstant(const string &name, bool value) = 0;
	virtual void setShaderConstant(const string &name, int value) = 0;
	virtual void setShaderConstant(const string &name, float value) = 0;
	virtual void setShaderConstant(const string &name, double value) = 0;
	virtual void setShaderConstant(const string &name, const Vector2df& value) = 0;
	virtual void setShaderConstant(const string &name, float x, float y) = 0;
	virtual void setShaderConstant(const string &name, const Vector3df& value) = 0;
	virtual void setShaderConstant(const string &name, float x, float y, float z) = 0;
	virtual void setShaderConstant(const string &name, float x, float y, float z, float w) = 0;
	virtual void setShaderConstant(const string &name, const Matrix4x4& value) = 0;
};

NS_VAYO3D_END

#endif // __VAYO3D_MATERIAL_RENDERER_H__
