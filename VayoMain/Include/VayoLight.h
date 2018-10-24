/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 灯光
\*************************************************************************/
#ifndef __VAYO_LIGHT_H__
#define __VAYO_LIGHT_H__

#include "VayoMovableObject.h"
#include "VayoLightData.h"
NS_VAYO_BEGIN

class _VayoExport Light
{
public:
	void setAmbient(int r, int g, int b, int a);
	void setDiffuse(int r, int g, int b, int a);
	void setSpecular(int r, int g, int b, int a);
protected:
	LightData _lightData;
};

/**
 * Serialized template:
 * <DirectionalLight name="主方向光" ambientColor="0xff808080" diffuseColor="0xffffffff" specularColor="ffcccccc" direction="-0.57735,0.57735,0.57735"/>
**/
class _VayoExport DirectionalLight : public MovableObject, public Light
{
public:
	DirectionalLight(const wstring& name);
	void update(float dt);
	void setDirection(float x, float y, float z);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

/**
 * Serialized template:
 * TODO
**/
class _VayoExport PointLight : public MovableObject, public Light
{
public:
	PointLight(const wstring& name);
	void update(float dt);
	void setPosition(float x, float y, float z);
	void setRange(float range);
	void setAtt(float a0, float a1, float a2);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

/**
 * Serialized template:
 * TODO
**/
class _VayoExport SpotLight : public PointLight
{
public:
	SpotLight(const wstring& name);
	void update(float dt);
	void setDirection(float x, float y, float z);
	void setExponent(float exponent);
	void setCutoff(float cutoff);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

NS_VAYO_END

#endif // __VAYO_LIGHT_H__
