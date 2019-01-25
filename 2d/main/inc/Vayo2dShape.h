#pragma once
#include "Vayo2dSupport.h"
#include "math/VayoVector2d.h"

NS_VAYO2D_BEGIN

class _Vayo2dExport Shape
{
public:
	Shape(unsigned int color);
	virtual ~Shape();
	virtual void update(float dt);
	virtual void render() = 0;

	void setPt(float x, float y, unsigned int idx = -1);
	void setPt(const Vector2df& pt, unsigned int idx = -1);
	void setPts(const vector<Vector2df>& pts);
	void addPts(const vector<Vector2df>& pts, unsigned int idx = -1);

	Vector2df& getPt(unsigned int idx = -1);
	const Vector2df& getPt(unsigned int idx = -1) const;
	vector<Vector2df>& getPts();
	const vector<Vector2df>& getPts() const;

	unsigned int getPtCount() const;
	void clearPts();

protected:
	vector<Vector2df> _points;
	ID2D1SolidColorBrush* _colorBrush;
};

NS_VAYO2D_END
