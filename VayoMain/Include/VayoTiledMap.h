/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2018 author by ��Ӻ�
* ��Ƭ��ͼ
\*************************************************************************/
#ifndef __VAYO_TILED_MAP_H__
#define __VAYO_TILED_MAP_H__

#include "VayoSupport.h"
#include "VayoTexture.h"
#include "VayoVector2d.h"
#include "VayoSharedPtr.h"

NS_VAYO_BEGIN

/**
 * TMXXMLParser: tmx(��ʵ��xml)��ʽ�ĵ�ͼ�ļ�������
**/
class TMXXMLParser
{
	friend class TiledMap;
	friend class TMXCSVLayer;

	TMXXMLParser();
	~TMXXMLParser();
	void destroyMapInfo();
	bool parseMapInfo(const wstring& strMapPath);

	struct tagLayer
	{
		wstring Name;
		int     Width;
		int     Height;
		string  GidData;
	};

	struct tagTileset
	{
		tagTileset() : BindTexture(NULL) {}
		int        FirstGid;
		int        LastGid;
		wstring    Name;
		int        TileWidth;
		int        TileHeight;
		int        TileCount;
		int        Columns;
		TexturePtr BindTexture;
		struct tagImage
		{
			wstring Source;
			int     Width;
			int     Height;
		} Img;
	};

	struct tagObjectgroup
	{
		wstring Name;
		struct tagObject
		{
			int     Id;
			wstring Name;
			wstring Type;
			float   X;
			float   Y;
			float   Width;
			float   Height;
		};
		vector<tagObject> Objs;
	};

	int                    _width;
	int                    _height;
	int                    _tilewidth;
	int                    _tileheight;
	vector<tagLayer>       _layers;
	vector<tagTileset>     _tilesets;
	vector<tagObjectgroup> _objectgroups;
};

/**
 * TMXCSVLayer: ���(������csv��ʽ����)
**/
class TMXCSVLayer
{
	friend class TiledMap;

	TMXCSVLayer();
	~TMXCSVLayer();
	void render();
	void initCSVLayer(TMXXMLParser* pMapInfo, unsigned curLayerIdx);
	void loadSprite(int gid, int vscRow, int csvCol, int tileWidth,
		int tileHeight, const TMXXMLParser::tagTileset& tileset);

	struct tagSpriteBatch
	{
		SpriteBox*          TheSprites;
		unsigned int        TexIdx;
		vector<Position2di> DstPos;
	};
	// ��ֵ��last gid.
	map<int, tagSpriteBatch> _batchSprites;
};

/**
 * TiledMap: ��Ƭ��ͼ
**/
class _VayoExport TiledMap
{
public:
	TiledMap(const wstring& name);
	virtual ~TiledMap();
	virtual void render();
	virtual bool init(const wstring& strMapPath);

private:
	TMXXMLParser         _tiledmapInfo;
	vector<TMXCSVLayer*> _tmxLayers;
};

NS_VAYO_END

#endif // __VAYO_TILED_MAP_H__