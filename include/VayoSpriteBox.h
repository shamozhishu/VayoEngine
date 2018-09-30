/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 精灵盒子
\*************************************************************************/
#ifndef __VAYO_SPRITE_BOX_H__
#define __VAYO_SPRITE_BOX_H__

#include "VayoSupport.h"
#include "VayoTexture.h"
#include "VayoRectangle.h"
NS_VAYO_BEGIN

struct tagSpriteFrame
{
	unsigned int TextureNumber;
	unsigned int RectNumber;
};

struct tagSprite
{
	tagSprite() : Frames(), FrameTime(0) {}
	vector<tagSpriteFrame> Frames;
	unsigned int           FrameTime;
};

class _VayoExport SpriteBox
{
public:
	SpriteBox();
	virtual ~SpriteBox();
	virtual vector<Recti>&     getPositions();
	virtual vector<tagSprite>& getSprites();
	virtual unsigned int       getTextureCount() const;
	virtual TexturePtr         getTexture(unsigned int index) const;
	virtual void               addTexture(TexturePtr texture);
	virtual void               setTexture(unsigned int index, TexturePtr texture);
	virtual int                addTextureAsSprite(TexturePtr texture);
	virtual void               clear();

	virtual void draw2DSprite(unsigned int index, const Position2di& pos, const Recti* clip = NULL,
		const Colour& color = Colour(255, 255, 255, 255),
		unsigned int starttime = 0, unsigned int currenttime = 0, bool loop = true, bool center = false);

	virtual void draw2DSpriteBatch(const vector<unsigned int>& indices, const vector<Position2di>& pos,
		const Recti* clip = NULL,
		const Colour& color = Colour(255, 255, 255, 255),
		unsigned int starttime = 0, unsigned int currenttime = 0,
		bool loop = true, bool center = false);

protected:
	struct tagDrawBatch
	{
		vector<Position2di> Positions;
		vector<Recti>       SourceRects;
		unsigned int        TextureIndex;
	};

	vector<tagSprite>  _sprites;
	vector<Recti>      _rectangles;
	vector<TexturePtr> _textures;
};

NS_VAYO_END

#endif // __VAYO_SPRITE_BOX_H__
