/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 灯光
\*************************************************************************/
#ifndef __VAYO3D_LIGHT_H__
#define __VAYO3D_LIGHT_H__

#include "Vayo3dMovableObject.h"
#include "Vayo3dLightData.h"
NS_VAYO3D_BEGIN

class _Vayo3dExport Light
{
public:
	void setAmbient(int r, int g, int b, int a);
	void setDiffuse(int r, int g, int b, int a);
	void setSpecular(int r, int g, int b, int a);
protected:
	LightData _lightData;
};

class _Vayo3dExport DirectionalLight : public MovableObject, public Light
{
	static Reflex<DirectionalLight, const wstring&, SceneManager*> _dynReflex;
public:
	DirectionalLight(const wstring& name, SceneManager* originSceneMgr);
	void update(float dt);
	void setDirection(float x, float y, float z);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

class _Vayo3dExport PointLight : public MovableObject, public Light
{
	static Reflex<PointLight, const wstring&, SceneManager*> _dynReflex;
public:
	PointLight(const wstring& name, SceneManager* originSceneMgr);
	void update(float dt);
	void setPosition(float x, float y, float z);
	void setRange(float range);
	void setAtt(float a0, float a1, float a2);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

class _Vayo3dExport SpotLight : public PointLight
{
	static Reflex<SpotLight, const wstring&, SceneManager*> _dynReflex;
public:
	SpotLight(const wstring& name, SceneManager* originSceneMgr);
	void update(float dt);
	void setDirection(float x, float y, float z);
	void setExponent(float exponent);
	void setCutoff(float cutoff);
	void serialize(XMLElement* outXml);
	bool deserialize(XMLElement* inXml);
};

NS_VAYO3D_END

#endif // __VAYO3D_LIGHT_H__
