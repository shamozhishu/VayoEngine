/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 authored by 朱加浩
* 灯光数据
\*************************************************************************/
#ifndef __VAYO_LIGHT_DATA_H__
#define __VAYO_LIGHT_DATA_H__

#include "VayoColour.h"
#include "VayoVector3d.h"
NS_VAYO_BEGIN

enum ELightType
{
	ELT_DIRECTIONAL_LIGHT = 0, // 方向光
	ELT_POINT_LIGHT,           // 点光源
	ELT_SPOT_LIGHT             // 聚光灯
};

class _VayoExport LightData
{
public:
	LightData() : _lightType((ELightType)-1) {}
	ELightType  _lightType;
	Colour      _ambientColor;
	Colour      _diffuseColor;
	Colour      _specularColor;
	Vector3df   _position;
	float       _range;
	Vector3df   _att;
	Vector3df   _direction;
	float       _exponent;
	float       _cutoff;
};

NS_VAYO_END

#endif // __VAYO_LIGHT_DATA_H__
