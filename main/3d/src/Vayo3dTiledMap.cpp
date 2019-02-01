#include "Vayo3dTiledMap.h"
#include "Vayo3dSpriteBox.h"
#include "VayoUtils.h"
#include "Vayo3dRoot.h"
#include "VayoLog.h"
#include "VayoConfigManager.h"
#include "Vayo3dTextureManager.h"
#include "math/VayoRectangle.h"
#include "tinyxml2/tinyxml2.h"

#define _TAG_TILESET_     "tileset"
#define _TAG_IMAGE_       "image"
#define _TAG_LAYER_       "layer"
#define _TAG_DATA_        "data"
#define _TAG_OBJECTGROUP_ "objectgroup"
#define _TAG_OBJECT_      "object"

using tinyxml2::XMLElement;

NS_VAYO3D_BEGIN

/************************************************************************/
/* TMXXMLParser ¿‡                                                      */
/************************************************************************/
TMXXMLParser::TMXXMLParser()
	: _width(0)
	, _height(0)
	, _tilewidth(0)
	, _tileheight(0)
{
}

TMXXMLParser::~TMXXMLParser()
{
	destroyMapInfo();
}

void TMXXMLParser::destroyMapInfo()
{
	unsigned int len = _tilesets.size();
	for (unsigned i = 0; i < len; ++i)
	{
		Root::getSingleton().getTextureManager()->removeTexture(_tilesets[i].BindTexture);
	}

	_width = 0;
	_height = 0;
	_tilewidth = 0;
	_tileheight = 0;
	_layers.clear();
	_tilesets.clear();
	_objectgroups.clear();
}

bool TMXXMLParser::parseMapInfo(const wstring& strMapPath)
{
	tinyxml2::XMLDocument doc;
	if (doc.LoadFile(w2a_(strMapPath).c_str()) != tinyxml2::XML_SUCCESS)
		return false;

	destroyMapInfo();
	const tagUIConfig& filePaths = ConfigManager::getSingleton().getUIConfig();
	string curTag;

	XMLElement* pRoot = doc.RootElement();
	if (NULL == pRoot)
		return false;

	_width = pRoot->IntAttribute("width");
	_height = pRoot->IntAttribute("height");
	_tilewidth = pRoot->IntAttribute("tilewidth");
	_tileheight = pRoot->IntAttribute("tileheight");

	XMLElement* pElem = pRoot->FirstChildElement();
	while (pElem)
	{
		curTag = pElem->Name();

		if (curTag == _TAG_TILESET_)
		{
			tagTileset tileset;
			tileset.FirstGid = pElem->IntAttribute("firstgid");
			tileset.Name = utf8ToUnicode(pElem->Attribute("name"));
			tileset.TileWidth = pElem->IntAttribute("tilewidth");
			tileset.TileHeight = pElem->IntAttribute("tileheight");
			tileset.TileCount = pElem->IntAttribute("tilecount");
			tileset.Columns = pElem->IntAttribute("columns");
			tileset.LastGid = tileset.FirstGid + tileset.TileCount - 1;
			_tilesets.push_back(tileset);

			XMLElement* pImg = pElem->FirstChildElement(_TAG_IMAGE_);
			while (pImg)
			{
				if (_tilesets.empty())
					break;

				tagTileset& tileSet = _tilesets.back();
				tileSet.Img.Source = filePaths.MapImgPath;
				tileSet.Img.Source += utf8ToUnicode(pImg->Attribute("source"));
				tileSet.Img.Width = pImg->IntAttribute("width");
				tileSet.Img.Height = pImg->IntAttribute("height");
				pImg = pImg->NextSiblingElement();
			}
		}
		else if (curTag == _TAG_LAYER_)
		{
			tagLayer layer;
			layer.Name = utf8ToUnicode(pElem->Attribute("name"));
			layer.Width = pElem->IntAttribute("width");
			layer.Height = pElem->IntAttribute("height");
			_layers.push_back(layer);

			XMLElement* pData = pElem->FirstChildElement(_TAG_DATA_);
			if (pData && !_layers.empty())
			{
				tagLayer& layer = _layers.back();
				const char* pszData = pData->GetText();
				layer.GidData = pszData;
			}
		}
		else if (curTag == _TAG_OBJECTGROUP_)
		{
			tagObjectgroup objectgroup;
			objectgroup.Name = utf8ToUnicode(pElem->Attribute("name"));
			_objectgroups.push_back(objectgroup);

			XMLElement* pObj = pElem->FirstChildElement(_TAG_OBJECT_);
			while (pObj)
			{
				if (_objectgroups.empty())
					break;

				tagObjectgroup& objectgroup = _objectgroups.back();
				tagObjectgroup::tagObject object;
				object.Id = pObj->IntAttribute("id");
				object.Name = utf8ToUnicode(pObj->Attribute("name"));
				object.Type = utf8ToUnicode(pObj->Attribute("type"));
				object.X = pObj->FloatAttribute("x");
				object.Y = pObj->FloatAttribute("y");
				object.Width = pObj->FloatAttribute("width");
				object.Height = pObj->FloatAttribute("height");
				objectgroup.Objs.push_back(object);
				pObj = pObj->NextSiblingElement();
			}
		}

		pElem = pElem->NextSiblingElement();
	}

	unsigned int len = _tilesets.size();
	for (unsigned i = 0; i < len; ++i)
	{
		_tilesets[i].BindTexture = Root::getSingleton().getTextureManager()->getTexture(_tilesets[i].Img.Source);
	}

	return true;
}

/************************************************************************/
/* TMXCSVLayer ¿‡                                                       */
/************************************************************************/
TMXCSVLayer::TMXCSVLayer()
{
}

TMXCSVLayer::~TMXCSVLayer()
{
	map<int, tagSpriteBatch>::iterator itor = _batchSprites.begin();
	for (; itor != _batchSprites.end(); ++itor)
	{
		delete itor->second.TheSprites;
	}
	_batchSprites.clear();
}

void TMXCSVLayer::render()
{
	vector<unsigned int> indices;
	map<int, tagSpriteBatch>::iterator itor = _batchSprites.begin();
	for (; itor != _batchSprites.end(); ++itor)
	{
		tagSpriteBatch& curBatch = itor->second;
		indices.clear();
		SpriteBox* pSpriteBox = curBatch.TheSprites;
		unsigned len = pSpriteBox->getSprites().size();
		indices.resize(len);
		for (unsigned i = 0; i < len; ++i)
		{
			indices[i] = i;
		}

		pSpriteBox->draw2DSpriteBatch(indices, curBatch.DstPos);
	}
}

void TMXCSVLayer::initCSVLayer(TMXXMLParser* pMapInfo, unsigned curLayerIdx)
{
	const vector<TMXXMLParser::tagTileset>& tileset = pMapInfo->_tilesets;
	const TMXXMLParser::tagLayer& layer = pMapInfo->_layers[curLayerIdx];
	unsigned charLen = layer.GidData.size() + 1;
	const char* pszData = layer.GidData.c_str();
	char buff[64] = {};
	int charOffset = 0;
	int rowColOffset = -1;

	for (unsigned i = 0; i < charLen; ++i)
	{
		if (',' == pszData[i] || '\0' == pszData[i])
		{
			buff[charOffset] = 0;
			int gid = atoi(buff);
			charOffset = 0;
			++rowColOffset;

			if (gid > 0)
			{
				unsigned len = tileset.size();
				for (unsigned j = 0; j < len; ++j)
				{
					if (gid >= tileset[j].FirstGid && gid <= tileset[j].LastGid)
					{
						loadSprite(gid, rowColOffset / layer.Width,
							rowColOffset%layer.Width, pMapInfo->_tilewidth,
							pMapInfo->_tileheight, tileset[j]);
						break;
					}
				}
			}

			continue;
		}

		buff[charOffset++] = pszData[i];
	}
}

void TMXCSVLayer::loadSprite(int gid, int vscRow, int csvCol, int tileWidth, int tileHeight,
	const TMXXMLParser::tagTileset& tileset)
{
	TexturePtr texture = tileset.BindTexture;
	if (NULL == texture)
		return;

	map<int, tagSpriteBatch>::iterator it = _batchSprites.find(tileset.LastGid);
	if (it == _batchSprites.end())
	{
		tagSpriteBatch batch;
		batch.TheSprites = new SpriteBox();
		_batchSprites.insert(make_pair(tileset.LastGid, batch));

		tagSpriteBatch& curBatch = _batchSprites[tileset.LastGid];
		curBatch.TheSprites->addTexture(texture);
		curBatch.TexIdx = curBatch.TheSprites->getTextureCount() - 1;
		it = _batchSprites.find(tileset.LastGid);
	}

	SpriteBox* pSpriteBox = it->second.TheSprites;
	tagSprite sprite;
	sprite.FrameTime = 0;

	Recti dstRect;
	dstRect._upperLeftCorner._x = csvCol * tileWidth;
	dstRect._upperLeftCorner._y = vscRow * tileHeight;
	dstRect._lowerRightCorner._x = dstRect._upperLeftCorner._x + tileWidth;
	dstRect._lowerRightCorner._y = dstRect._upperLeftCorner._y + tileHeight;
	it->second.DstPos.push_back(Position2di(csvCol * tileWidth, vscRow * tileHeight));

	Recti srcRect;
	int rowNum = tileset.TileCount / tileset.Columns;
	int oriImgRow = (gid - tileset.FirstGid) / tileset.Columns;
	int oriImgCol = (gid - tileset.FirstGid) % tileset.Columns;
	int oriImgTileW = tileset.Img.Width / tileset.Columns;
	int oriImgTileH = tileset.Img.Height / rowNum;
	srcRect._upperLeftCorner._x = oriImgCol * oriImgTileW;
	srcRect._upperLeftCorner._y = oriImgRow * oriImgTileH;
	srcRect._lowerRightCorner._x = srcRect._upperLeftCorner._x + oriImgTileW;
	srcRect._lowerRightCorner._y = srcRect._upperLeftCorner._y + oriImgTileH;
	pSpriteBox->getPositions().push_back(srcRect);

	tagSpriteFrame frame;
	frame.TextureNumber = it->second.TexIdx;
	frame.RectNumber = pSpriteBox->getPositions().size() - 1;
	sprite.Frames.push_back(frame);

	pSpriteBox->getSprites().push_back(sprite);
}

/************************************************************************/
/* TiledMap ¿‡                                                          */
/************************************************************************/
TiledMap::TiledMap(const wstring& name)
{
}

TiledMap::~TiledMap()
{
	unsigned len = _tmxLayers.size();
	for (unsigned i = 0; i < len; ++i)
	{
		SAFE_DELETE(_tmxLayers[i]);
	}

	_tmxLayers.clear();
}

void TiledMap::render()
{
	unsigned len = _tmxLayers.size();
	for (unsigned i = 0; i < len; ++i)
	{
		_tmxLayers[i]->render();
	}
}

bool TiledMap::init(const wstring& strMapPath)
{
	if (!_tiledmapInfo.parseMapInfo(strMapPath))
		return false;

	unsigned len = _tiledmapInfo._layers.size();
	for (unsigned i = 0; i < len; ++i)
	{
		_tmxLayers.push_back(new TMXCSVLayer());
		_tmxLayers.back()->initCSVLayer(&_tiledmapInfo, i);
	}

	Rectf mapLocation;
	len = _tiledmapInfo._objectgroups.size();
	for (unsigned i = 0; i < len; ++i)
	{
		unsigned objNum = _tiledmapInfo._objectgroups[i].Objs.size();
		for (unsigned j = 0; j < objNum; ++j)
		{
			const TMXXMLParser::tagObjectgroup::tagObject& obj = _tiledmapInfo._objectgroups[i].Objs[j];
			mapLocation._upperLeftCorner._x = obj.X;
			mapLocation._upperLeftCorner._y = obj.Y - obj.Height;
			mapLocation._lowerRightCorner._x = obj.X + obj.Width;
			mapLocation._lowerRightCorner._y = obj.Y;
			// dadawa.
		}
	}

	return true;
}

NS_VAYO3D_END
