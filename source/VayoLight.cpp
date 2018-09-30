#include "VayoLight.h"
#include "VayoSceneNode.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "tinyxml2/tinyxml2.h"
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

bool DirectionalLight::parseXML(XMLElement* xml)
{
	if (!xml)
		return false;

	string strTemp = xml->Attribute("ambientColor");
	_lightData._ambientColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = xml->Attribute("diffuseColor");
	_lightData._diffuseColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = xml->Attribute("specularColor");
	_lightData._specularColor.set(strtoul(strTemp.c_str(), NULL, 16));

	vector<string> container;
	strTemp = xml->Attribute("direction");
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

bool PointLight::parseXML(XMLElement* xml)
{
	if (!xml)
		return false;

	string strTemp = xml->Attribute("ambientColor");
	_lightData._ambientColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = xml->Attribute("diffuseColor");
	_lightData._diffuseColor.set(strtoul(strTemp.c_str(), NULL, 16));
	strTemp = xml->Attribute("specularColor");
	_lightData._specularColor.set(strtoul(strTemp.c_str(), NULL, 16));

	_lightData._range = xml->FloatAttribute("range");
	vector<string> container;
	strTemp = xml->Attribute("att");
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

bool SpotLight::parseXML(XMLElement* xml)
{
	if (!PointLight::parseXML(xml))
		return false;

	_lightData._exponent = xml->FloatAttribute("exponent");
	_lightData._cutoff = xml->FloatAttribute("cutoff");
	vector<string> container;
	string strTemp = xml->Attribute("direction");
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
