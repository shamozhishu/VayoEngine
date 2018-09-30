/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* TrueType字体
\*************************************************************************/
#ifndef __VAYO_FONT_MANAGER_H__
#define __VAYO_FONT_MANAGER_H__

#include "VayoSupport.h"
#include "VayoTexture.h"
#include "VayoSharedPtr.h"
// FreeType library
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_OUTLINE_H

#ifdef CONVERT_FONT_INDEX
#undef CONVERT_FONT_INDEX
#endif
#define CONVERT_FONT_INDEX(x) (((x) < 0 || (x) >= (int)_fonts.size()) ? -1 : (x))

NS_VAYO_BEGIN

const int TRUE_TYPE_FONT_TEXTURE_SIZE = 256;

/** 字体管理器 */
class FontManager
{
public:
	FontManager();
	~FontManager();
	bool initialize();
	void release();
	int  createFont(const char* filename, int face, int tall, bool bold, bool italic, bool antialias);
	bool getCharInfo(int fontIndex, int code, int* wide, int* tall, int* bearingX, int* bearingY, int* advance, TexturePtr& texture, float coords[]);
	int  getFontTall(int fontIndex);

private:
	struct tagGlyphMetrics
	{
		int    Width;
		int    Height;
		int    BearingX;
		int    BearingY;
		int    Advance;
	};

	class TrueTypeFont
	{
	public:
		TrueTypeFont();
		~TrueTypeFont();
		bool create(FT_Library library, const char* filename, FT_Long faceIndex, int tall, bool bold, bool italic, bool antialias);
		void release();
		bool getCharInfo(int code, tagGlyphMetrics* metrics, TexturePtr& texture, float coords[]);
		int  getFontTall();

	private:
		bool loadChar(int code, tagGlyphMetrics* metrics);

		class Char
		{
		public:
			void setInfo(tagGlyphMetrics* metrics);
			void getInfo(tagGlyphMetrics* metrics, TexturePtr& texture, float coords[]);

		public:
			int             _code;
			TexturePtr      _texture;
			float           _coords[4]; // left top right bottom

		private:
			tagGlyphMetrics _metrics;
		};

		class Page
		{
		public:
			Page();
			~Page();
			bool         append(int wide, int tall, unsigned char* rgba, float coords[]);
			TexturePtr   getTexture();

		private:
			TexturePtr   _texture;
			int          _wide;
			int          _tall;
			int          _posx;
			int          _posy;
			int          _maxCharTall;
		};

		typedef map<int, Char*> TCharMap;
		FT_Library              _library;
		FT_Face                 _face;
		bool                    _antialias;
		bool                    _bold;
		int                     _tall;
		int                     _rgbaSize;
		unsigned char*          _rgba;
		TCharMap                _chars;
		vector<Page*>           _pages;
	};

	FT_Library                  _library;
	vector<TrueTypeFont*>       _fonts;
};

NS_VAYO_END

#endif // __VAYO_FONT_MANAGER_H__
