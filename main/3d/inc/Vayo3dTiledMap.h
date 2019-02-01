/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 瓦片地图
\*************************************************************************/
#ifndef __VAYO3D_TILED_MAP_H__
#define __VAYO3D_TILED_MAP_H__

#include "Vayo3dSupport.h"
#include "Vayo3dTexture.h"
#include "VayoSharedPtr.h"
#include "math/VayoVector2d.h"

NS_VAYO3D_BEGIN

/**
 * TMXXMLParser: tmx(其实是xml)格式的地图文件解析器
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
 * TMXCSVLayer: 块层(数据用csv方式编码)
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
	// 键值是last gid.
	map<int, tagSpriteBatch> _batchSprites;
};

/**
 * TiledMap: 瓦片地图
**/
class _Vayo3dExport TiledMap
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

NS_VAYO3D_END

#endif // __VAYO3D_TILED_MAP_H__
