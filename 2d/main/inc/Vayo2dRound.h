#pragma once
#include "Vayo2dShape.h"

NS_VAYO2D_BEGIN

class _Vayo2dExport Round : public Shape
{
public:
	Round(float x, float y, float diameter, unsigned int color, ID2D1Factory* factory);
	~Round();
	void render();

protected:
	ID2D1EllipseGeometry* _roundGeometry;
};

NS_VAYO2D_END
