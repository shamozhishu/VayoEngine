#include "VayoSpriteBox.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"

NS_VAYO_BEGIN

unsigned int SpriteBox::getTextureCount() const
{
	return _textures.size();
}

SpriteBox::SpriteBox()
{
}

SpriteBox::~SpriteBox()
{
	clear();
}

std::vector<Recti>& SpriteBox::getPositions()
{
	return _rectangles;
}

std::vector<tagSprite>& SpriteBox::getSprites()
{
	return _sprites;
}

TexturePtr SpriteBox::getTexture(unsigned int index) const
{
	if (index < _textures.size())
		return _textures[index];
	return NULL;
}

void SpriteBox::addTexture(TexturePtr texture)
{
	if (texture)
		_textures.push_back(texture);
}

void SpriteBox::setTexture(unsigned int index, TexturePtr texture)
{
	while (index >= _textures.size())
		_textures.push_back(0);
	_textures[index] = texture;
}

int SpriteBox::addTextureAsSprite(TexturePtr texture)
{
	if (!texture)
		return -1;

	addTexture(texture);
	unsigned int textureIndex = getTextureCount() - 1;

	unsigned int rectangleIndex = _rectangles.size();
	_rectangles.push_back(Recti(0, 0, texture->getImgSize()._width, texture->getImgSize()._height));

	tagSprite sprite;
	sprite.FrameTime = 0;

	tagSpriteFrame frame;
	frame.TextureNumber = textureIndex;
	frame.RectNumber = rectangleIndex;
	sprite.Frames.push_back(frame);

	_sprites.push_back(sprite);

	return _sprites.size() - 1;
}

void SpriteBox::clear()
{
	_sprites.clear();
	_rectangles.clear();
	_textures.clear();
}

void SpriteBox::draw2DSprite(unsigned int index, const Position2di& pos,
	const Recti* clip /*= NULL*/, const Colour& color /*= Colour(255, 255, 255, 255)*/,
	unsigned int starttime /*= 0*/, unsigned int currenttime /*= 0*/,
	bool loop /*= true*/, bool center /*= false*/)
{
	if (index >= _sprites.size() || _sprites[index].Frames.empty())
		return;

	unsigned int frame = 0;
	if (_sprites[index].FrameTime)
	{
		unsigned int f = ((currenttime - starttime) / _sprites[index].FrameTime);
		if (loop)
			frame = f % _sprites[index].Frames.size();
		else
			frame = (f >= _sprites[index].Frames.size()) ? _sprites[index].Frames.size() - 1 : f;
	}

	const TexturePtr tex = _textures[_sprites[index].Frames[frame].TextureNumber];
	if (!tex)
		return;

	const unsigned int rn = _sprites[index].Frames[frame].RectNumber;
	if (rn >= _rectangles.size())
		return;

	const Recti& r = _rectangles[rn];

	if (center)
	{
		Position2di p = pos;
		Dimension2di siz = r.getSize() / 2;
		p -= Position2di(siz._width, siz._height);
		Root::getSingleton().getActiveRenderer()->draw2DImage(tex, p, r, clip, color, true);
	}
	else
	{
		Root::getSingleton().getActiveRenderer()->draw2DImage(tex, pos, r, clip, color, true);
	}
}

void SpriteBox::draw2DSpriteBatch(const vector<unsigned int>& indices,
	const vector<Position2di>& pos, const Recti* clip /*= NULL*/,
	const Colour& color /*= Colour(255, 255, 255, 255)*/, unsigned int starttime /*= 0*/,
	unsigned int currenttime /*= 0*/, bool loop /*= true*/, bool center /*= false*/)
{
	const unsigned int drawCount = min_<unsigned int>(indices.size(), pos.size());

	if (_textures.empty())
		return;

	vector<tagDrawBatch> drawBatches(_textures.size());
	for (unsigned int i = 0; i < _textures.size(); i++)
	{
		drawBatches[i].Positions.reserve(drawCount);
		drawBatches[i].SourceRects.reserve(drawCount);
		drawBatches[i].TextureIndex = i;
	}

	for (unsigned int i = 0; i < drawCount; ++i)
	{
		const unsigned int index = indices[i];

		if (index >= _sprites.size() || _sprites[index].Frames.empty())
			continue;

		unsigned int frame = 0;
		if (_sprites[index].FrameTime)
		{
			unsigned int f = ((currenttime - starttime) / _sprites[index].FrameTime);
			if (loop)
				frame = f % _sprites[index].Frames.size();
			else
				frame = (f >= _sprites[index].Frames.size()) ? _sprites[index].Frames.size() - 1 : f;
		}

		const unsigned int texNum = _sprites[index].Frames[frame].TextureNumber;

		tagDrawBatch& currentBatch = drawBatches[texNum];

		const unsigned int rn = _sprites[index].Frames[frame].RectNumber;
		if (rn >= _rectangles.size())
			continue;

		const Recti& r = _rectangles[rn];

		if (center)
		{
			Position2di p = pos[i];
			Dimension2di siz = r.getSize() / 2;
			p -= Position2di(siz._width, siz._height);

			currentBatch.Positions.push_back(p);
			currentBatch.SourceRects.push_back(r);
		}
		else
		{
			currentBatch.Positions.push_back(pos[i]);
			currentBatch.SourceRects.push_back(r);
		}
	}

	for (unsigned int i = 0; i < drawBatches.size(); ++i)
	{
		if (!drawBatches[i].Positions.empty() && !drawBatches[i].SourceRects.empty())
			Root::getSingleton().getActiveRenderer()->draw2DImageBatch(_textures[i], drawBatches[i].Positions,
				drawBatches[i].SourceRects, clip, color, true);
	}
}

NS_VAYO_END
