/*************************************************************************\
* Õ˚”—“˝«Ê1.0
* Copyright (c) 2018-2019 authored by ÷Ïº”∫∆
* ≤ƒ÷ ‰÷»æ
\*************************************************************************/
#ifndef __VAYO_MATERIAL_RENDERER_H__
#define __VAYO_MATERIAL_RENDERER_H__

#include "VayoSupport.h"
#include "VayoMaterial.h"
#include "VayoVector2d.h"
#include "VayoVector3d.h"
#include "VayoMatrix4x4.h"
NS_VAYO_BEGIN

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

NS_VAYO_END

#endif // __VAYO_MATERIAL_RENDERER_H__
