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
	, _material(Root::getSingleton().getMaterialManager()->createMaterial())
	, _selfMaterialName(_material->_materialName)
{
}

Renderable::~Renderable()
{
	Root::getSingleton().getMaterialManager()->destroyMaterial(_selfMaterialName);
}

MaterialPtr Renderable::getMaterial() const
{
	return _material;
}

void Renderable::setMaterial(MaterialPtr material)
{
	if (!material)
	{
		_material = Root::getSingleton().getMaterialManager()->findMaterial(_selfMaterialName);
		return;
	}

	if (_material.get() != material.get())
		_material = material;
}

void Renderable::setMaterial(const wstring& name)
{
	setMaterial(Root::getSingleton().getMaterialManager()->findMaterial(name));
}

NS_VAYO3D_END
