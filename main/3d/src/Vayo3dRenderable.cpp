#include "Vayo3dRenderable.h"
#include "Vayo3dRoot.h"
#include "Vayo3dMaterialManager.h"

NS_VAYO3D_BEGIN

void Renderable::getWorldTransform(Matrix4x4& mat) const
{
	mat = IdentityMat4;
}

Renderable::Renderable()
	: _renderPriority((unsigned int)-1)
{
	_material = Root::getSingleton().getMaterialManager()->getDefaultMaterial();
}

MaterialPtr& Renderable::getMaterial()
{
	if (!_material)
		_material = Root::getSingleton().getMaterialManager()->getDefaultMaterial();
	return _material;
}

const MaterialPtr& Renderable::getMaterial() const
{
	if (!_material)
		_material = Root::getSingleton().getMaterialManager()->getDefaultMaterial();
	return _material;
}

void Renderable::setMaterial(const wstring& name)
{
	if (getMaterial()->_materialName != name)
		_material = Root::getSingleton().getMaterialManager()->findMaterial(name);
}

void Renderable::setMaterial(const MaterialPtr& material)
{
	if (!material)
	{
		_material = Root::getSingleton().getMaterialManager()->getDefaultMaterial();
		return;
	}

	if (getMaterial().get() != material.get())
		_material = material;
}

NS_VAYO3D_END
