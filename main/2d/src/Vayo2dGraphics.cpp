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
	, _feature(Pivot::getSingleton().getFeatureManager()->createFeature())
	, _selfFeatureName(_feature->_featureName)
{
}

Graphics::~Graphics()
{
	Pivot::getSingleton().getFeatureManager()->destroyFeature(_selfFeatureName);
}

FeaturePtr Graphics::getFeature() const
{
	return _feature;
}

void Graphics::setFeature(FeaturePtr feature)
{
	if (!feature)
	{
		_feature = Pivot::getSingleton().getFeatureManager()->findFeature(_selfFeatureName);
		return;
	}

	if (_feature.get() != feature.get())
		_feature = feature;
}

void Graphics::setFeature(const wstring& name)
{
	setFeature(Pivot::getSingleton().getFeatureManager()->findFeature(name));
}

NS_VAYO2D_END
