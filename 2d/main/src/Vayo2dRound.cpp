#include "Vayo2dRound.h"

NS_VAYO2D_BEGIN

void Round::render()
{
}

Round::Round(float x, float y, float diameter, unsigned int color, ID2D1Factory* factory)
	: Shape(color)
{
}

Round::~Round()
{
	safeRelease(_roundGeometry);
}

NS_VAYO2D_END
