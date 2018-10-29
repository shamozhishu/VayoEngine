#include "VayoLight.h"
#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

void Light::setAmbient(int r, int g, int b, int a)
{
	_lightData._ambientColor.set(a, r, g, b);
}

void Light::setDiffuse(int r, int g, int b, int a)
{
	_lightData._diffuseColor.set(a, r, g, b);
}

void Light::setSpecular(int r, int g, int b, int a)
{
	_lightData._specularColor.set(a, r, g, b);
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_WITHPARA_IMPLEMENT(DirectionalLight, const wstring&)
DirectionalLight::DirectionalLight(const wstring& name)
	: MovableObject(name)
{
	_lightData._lightType = ELT_DIRECTIONAL_LIGHT;
}

void DirectionalLight::update(float dt)
{
	SceneNode* pParent = getParentNode();
	if (pParent)
	{
		_lightData._position = pParent->getAbsPosition();
		Root::getSingleton().getActiveRenderer()->addDynamicLight(_lightData);
	}
}

void DirectionalLight::setDirection(float x, float y, float z)
{
	_lightData._direction.set(x, y, z);
}

void DirectionalLight::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	char szbuf[256];
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._ambientColor._clr);
	outXml->SetAttribute("ambientColor", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._diffuseColor._clr);
	outXml->SetAttribute("diffuseColor", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._specularColor._clr);
	outXml->SetAttribute("specularColor", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _lightData._direction._x, _lightData._direction._y, _lightData._direction._z);
	outXml->SetAttribute("direction", szbuf);
}

bool DirectionalLight::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	string strTemp = inXml->Attribute("ambientColor");
	_lightData._ambientColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = inXml->Attribute("diffuseColor");
	_lightData._diffuseColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = inXml->Attribute("specularColor");
	_lightData._specularColor.set(strtoul(strTemp.c_str(), NULL, 16));

	vector<string> container;
	strTemp = inXml->Attribute("direction");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		_lightData._direction.set(x, y, z);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_WITHPARA_IMPLEMENT(PointLight, const wstring&)
PointLight::PointLight(const wstring& name)
	: MovableObject(name)
{
	_lightData._lightType = ELT_POINT_LIGHT;
}

void PointLight::update(float dt)
{
	SceneNode* pParent = getParentNode();
	if (pParent)
	{
		_lightData._position = pParent->getAbsPosition();
		Root::getSingleton().getActiveRenderer()->addDynamicLight(_lightData);
	}
}

void PointLight::setPosition(float x, float y, float z)
{
	SceneNode* pParent = getParentNode();
	if (pParent)
		pParent->setPosition(Vector3df(x, y, z));
}

void PointLight::setRange(float range)
{
	_lightData._range = range;
}

void PointLight::setAtt(float a0, float a1, float a2)
{
	_lightData._att.set(a0, a1, a2);
}

void PointLight::serialize(XMLElement* outXml)
{
	MovableObject::serialize(outXml);
	char szbuf[256];
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._ambientColor._clr);
	outXml->SetAttribute("ambientColor", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._diffuseColor._clr);
	outXml->SetAttribute("diffuseColor", szbuf);
	sprintf_s(szbuf, sizeof(szbuf), "0x%08x", _lightData._specularColor._clr);
	outXml->SetAttribute("specularColor", szbuf);
	outXml->SetAttribute("range", _lightData._range);
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _lightData._att._x, _lightData._att._y, _lightData._att._z);
	outXml->SetAttribute("att", szbuf);
}

bool PointLight::deserialize(XMLElement* inXml)
{
	if (!MovableObject::deserialize(inXml))
		return false;

	string strTemp = inXml->Attribute("ambientColor");
	_lightData._ambientColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = inXml->Attribute("diffuseColor");
	_lightData._diffuseColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = inXml->Attribute("specularColor");
	_lightData._specularColor.set(strtoul(strTemp.c_str(), NULL, 16));

	_lightData._range = inXml->FloatAttribute("range");
	vector<string> container;
	strTemp = inXml->Attribute("att");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float a0 = (float)atof(container[0].c_str());
		float a1 = (float)atof(container[1].c_str());
		float a2 = (float)atof(container[2].c_str());
		_lightData._att.set(a0, a1, a2);
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
VAYO_REFLEX_WITHPARA_IMPLEMENT(SpotLight, const wstring&)
SpotLight::SpotLight(const wstring& name)
	: PointLight(name)
{
	_lightData._lightType = ELT_SPOT_LIGHT;
}

void SpotLight::update(float dt)
{
	SceneNode* pParent = getParentNode();
	if (pParent)
	{
		_lightData._position = pParent->getAbsPosition();
		Root::getSingleton().getActiveRenderer()->addDynamicLight(_lightData);
	}
}

void SpotLight::setDirection(float x, float y, float z)
{
	_lightData._direction.set(x, y, z);
}

void SpotLight::setExponent(float exponent)
{
	_lightData._exponent = exponent;
}

void SpotLight::setCutoff(float cutoff)
{
	_lightData._cutoff = cutoff;
}

void SpotLight::serialize(XMLElement* outXml)
{
	PointLight::serialize(outXml);
	outXml->SetAttribute("exponent", _lightData._exponent);
	outXml->SetAttribute("cutoff", _lightData._cutoff);
	char szbuf[256];
	sprintf_s(szbuf, sizeof(szbuf), "%f,%f,%f", _lightData._direction._x, _lightData._direction._y, _lightData._direction._z);
	outXml->SetAttribute("direction", szbuf);
}

bool SpotLight::deserialize(XMLElement* inXml)
{
	if (!PointLight::deserialize(inXml))
		return false;

	_lightData._exponent = inXml->FloatAttribute("exponent");
	_lightData._cutoff = inXml->FloatAttribute("cutoff");
	vector<string> container;
	string strTemp = inXml->Attribute("direction");
	stringtok(container, strTemp, ",");
	if (container.size() >= 3)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		float z = (float)atof(container[2].c_str());
		_lightData._direction.set(x, y, z);
	}

	return true;
}

NS_VAYO_END
