#include "Vayo2dGraphics.h"
#include "Vayo2dPivot.h"
#include "Vayo2dFeatureManager.h"

NS_VAYO2D_BEGIN

void Graphics::getWorldTransform(Matrix3x3& mat) const
{
	mat = IdentityMat3;
}

Graphics::Graphics()
	: _zorder((unsigned int)-1)
{
	_feature = Pivot::getSingleton().getFeatureManager()->getDefaultFeature();
}

FeaturePtr& Graphics::getFeature()
{
	if (!_feature)
		_feature = Pivot::getSingleton().getFeatureManager()->getDefaultFeature();
	return _feature;
}

const FeaturePtr& Graphics::getFeature() const
{
	if (!_feature)
		_feature = Pivot::getSingleton().getFeatureManager()->getDefaultFeature();
	return _feature;
}

void Graphics::setFeature(const wstring& name)
{
	if (getFeature()->_featureName != name)
		_feature = Pivot::getSingleton().getFeatureManager()->findFeature(name);
}

void Graphics::setFeature(const FeaturePtr& feature)
{
	if (!feature)
	{
		_feature = Pivot::getSingleton().getFeatureManager()->getDefaultFeature();
		return;
	}

	if (getFeature().get() != feature.get())
		_feature = feature;
}

NS_VAYO2D_END
