#include "Vayo2dShape.h"

NS_VAYO2D_BEGIN

void Shape::getWorldTransform(Matrix3x3& mat) const
{
	mat = IdentityMat3;
}

Shape::Shape()
	: _renderPriority((unsigned int)-1)
{

}

Shape::~Shape()
{
}

FeaturePtr& Shape::getFeature()
{
	return _feature;
}

const FeaturePtr& Shape::getFeature() const
{
	return _feature;
}

void Shape::setFeature(const wstring& name)
{

}

void Shape::setFeature(const FeaturePtr& feature)
{

}

NS_VAYO2D_END
