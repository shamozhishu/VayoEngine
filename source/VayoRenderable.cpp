#include "VayoRenderable.h"
#include "VayoRoot.h"
#include "VayoMaterialManager.h"

NS_VAYO_BEGIN

void Renderable::getWorldTransform(Matrix4x4& mat) const
{
	mat = IdentityMatrix;
}

Renderable::Renderable()
	: _renderPriority((unsigned int)-1)
{
	_material = Root::getSingleton().getMaterialManager()->getDefaultMaterial();
}

MaterialPtr& Renderable::getMaterial()
{
	return _material;
}

const MaterialPtr& Renderable::getMaterial() const
{
	return _material;
}

void Renderable::setMaterial(const wstring& name)
{
	_material = Root::getSingleton().getMaterialManager()->findMaterial(name);
}

void Renderable::setMaterial(const MaterialPtr& material)
{
	_material = material;
}

NS_VAYO_END
