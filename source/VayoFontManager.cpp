#include "VayoFontManager.h"
#include "VayoLog.h"
#include "VayoRoot.h"
#include "VayoRenderSystem.h"
#include "VayoTextureManager.h"

NS_VAYO_BEGIN

//------------------------------------------------------------------------
// bitmap.width  位图宽度
// bitmap.rows   位图行数(高度)
// bitmap.pitch  位图一行占用的字节数
//------------------------------------------------------------------------

// MONO模式每1个像素仅用1bit保存,只有黑和白.
// 1个byte可以保存8个像素,1个int可以保存8*4个像素.
static inline void convertMONOToRGBA(FT_Bitmap* source, unsigned char* rgba)
{
	unsigned char *s = source->buffer;
	unsigned char *t = rgba;

	for (unsigned int y = source->rows; y > 0; y--)
	{
		unsigned char *ss = s;
		unsigned char *tt = t;

		for (unsigned int x = source->width >> 3; x > 0; x--)
		{
			unsigned int val = *ss;

			for (unsigned int i = 8; i > 0; i--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = (val & (1 << (i - 1))) ? 0xFF : 0x00;
				tt += 4;
			}

			ss += 1;
		}

		unsigned int rem = source->width & 7;

		if (rem > 0)
		{
			unsigned int val = *ss;

			for (unsigned int x = rem; x > 0; x--)
			{
				tt[0] = tt[1] = tt[2] = tt[3] = (val & 0x80) ? 0xFF : 0x00;
				tt += 4;
				val <<= 1;
			}
		}

		s += source->pitch;
		t += source->width * 4; // pitch
	}
}

// GRAY模式1个像素用1个字节保存.
static inline void convertGRAYToRGBA(FT_Bitmap* source, unsigned char* rgba)
{
	for (unsigned int y = 0; y < source->rows; y++)
	{
		for (unsigned int x = 0; x < source->width; x++)
		{
			unsigned char *s = &source->buffer[(y * source->pitch) + x];
			unsigned char *t = &rgba[((y * source->pitch) + x) * 4];

			t[0] = t[1] = t[2] = 0xFF;
			t[3] = *s;
		}
	}
}

//------------------------------------------------------------------------
// TrueTypeFont
//------------------------------------------------------------------------
FontManager::TrueTypeFont::TrueTypeFont()
	: _face(NULL)
	, _rgba(NULL)
	, _antialias(false)
	, _bold(false)
	, _tall(0)
{
}

FontManager::TrueTypeFont::~TrueTypeFont()
{
	release();
}

bool FontManager::TrueTypeFont::create(FT_Library library, const char* filename,
	FT_Long faceIndex, int tall, bool bold, bool italic, bool antialias)
{
	FT_Error err;

	if (tall > TRUE_TYPE_FONT_TEXTURE_SIZE)
	{
		// Bigger than a page size
		return false;
	}

	if ((err = FT_New_Face(library, filename, faceIndex, &_face)) != FT_Err_Ok)
	{
		Log::print(ELL_ERROR, "FT_New_Face() Error %d", err);
		return false;
	}

	if ((err = FT_Set_Pixel_Sizes(_face, 0, tall)) != FT_Err_Ok)
	{
		Log::print(ELL_ERROR, "FT_Set_Pixel_Sizes() Error %d", err);
		return false;
	}

	_rgbaSize = (tall * 2) * tall * 4;

	_rgba = new unsigned char[_rgbaSize];

	if (_rgba == NULL)
	{
		return false;
	}

	_library = library;
	_antialias = antialias;
	_bold = bold;
	_tall = tall;

	if (italic)
	{
		FT_Matrix m;
		m.xx = 0x10000L;
		m.xy = 0.5f * 0x10000L;
		m.yx = 0;
		m.yy = 0x10000L;
		FT_Set_Transform(_face, &m, NULL);
	}

	return true;
}

void FontManager::TrueTypeFont::release()
{
	FT_Error err;

	if (_face)
	{
		if ((err = FT_Done_Face(_face)) != FT_Err_Ok)
		{
			Log::print(ELL_ERROR, "FT_Done_Face() Error %d", err);
		}
		_face = NULL;
	}

	if (_rgba)
	{
		delete[] _rgba;
		_rgba = NULL;
	}

	for (TCharMap::iterator it = _chars.begin(); it != _chars.end(); it++)
	{
		delete it->second;
		it->second = NULL;
	}

	_chars.clear();

	for (size_t i = 0; i < _pages.size(); i++)
	{
		delete _pages[i];
		_pages[i] = NULL;
	}

	_pages.clear();
}

bool FontManager::TrueTypeFont::getCharInfo(int code, tagGlyphMetrics* metrics,
	TexturePtr& texture, float coords[])
{
	// fast find it
	TCharMap::iterator it = _chars.find(code);

	if (it != _chars.end())
	{
		it->second->getInfo(metrics, texture, coords);
		return true;
	}

	tagGlyphMetrics gm;

	if (loadChar(code, &gm) == false)
	{
		return false;
	}

	Char *ch = new Char();

	ch->_code = code;
	ch->setInfo(&gm);

	for (size_t i = 0; i < _pages.size(); i++)
	{
		Page *page = _pages[i];

		if (page->append(gm.Width, gm.Height, _rgba, ch->_coords))
		{
			ch->_texture = page->getTexture();
			ch->getInfo(metrics, texture, coords);
			_chars.insert(TCharMap::value_type(code, ch));
			return true;
		}
	}

	Page *page = new Page();

	if (page->append(gm.Width, gm.Height, _rgba, ch->_coords))
	{
		ch->_texture = page->getTexture();
		ch->getInfo(metrics, texture, coords);
		_chars.insert(TCharMap::value_type(code, ch));
		_pages.push_back(page);
		return true;
	}

	delete ch;
	delete page;

	return false;
}

int FontManager::TrueTypeFont::getFontTall()
{
	return _tall;
}

bool FontManager::TrueTypeFont::loadChar(int code, tagGlyphMetrics* metrics)
{
	FT_Error err;

	FT_UInt glyph_index = FT_Get_Char_Index(_face, (FT_ULong)code);

	if (glyph_index > 0)
	{
		if ((err = FT_Load_Glyph(_face, glyph_index, FT_LOAD_DEFAULT)) == FT_Err_Ok)
		{
			FT_GlyphSlot glyph = _face->glyph;

			FT_Render_Mode render_mode = _antialias ? FT_RENDER_MODE_NORMAL : FT_RENDER_MODE_MONO;

			if (_antialias && _bold)
			{
				if ((err = FT_Outline_EmboldenXY(&glyph->outline, 60, 60)) != FT_Err_Ok)
				{
					Log::print(ELL_ERROR, "FT_Outline_EmboldenXY() Error %d", err);
				}
			}

			if ((err = FT_Render_Glyph(glyph, render_mode)) == FT_Err_Ok)
			{
				FT_Bitmap* bitmap = &glyph->bitmap;

				switch (bitmap->pixel_mode)
				{
				case FT_PIXEL_MODE_MONO:
					{
						if (!_antialias && _bold)
						{
							if ((err = FT_Bitmap_Embolden(_library, bitmap, 60, 0)) != FT_Err_Ok)
							{
								Log::print(ELL_ERROR, "FT_Bitmap_Embolden() Error %d", err);
							}
						}
						convertMONOToRGBA(bitmap, _rgba);
						break;
					}
				case FT_PIXEL_MODE_GRAY:
					{
						convertGRAYToRGBA(bitmap, _rgba);
						break;
					}
				default:
					{
						memset(_rgba, 0xFF, _rgbaSize);
						break;
					}
				}

				metrics->Width = bitmap->width;
				metrics->Height = bitmap->rows;
				metrics->BearingX = glyph->bitmap_left;
				metrics->BearingY = glyph->bitmap_top;
				metrics->Advance = glyph->advance.x >> 6;

				return true;
			}
			else
			{
				Log::print(ELL_ERROR, "FT_Render_Glyph() Error %d", err);
			}
		}
		else
		{
			Log::print(ELL_ERROR, "FT_Load_Glyph() Error %d", err);
		}
	}

	memset(metrics, 0, sizeof(tagGlyphMetrics));
	return false;
}

//------------------------------------------------------------------------
// Char
//------------------------------------------------------------------------
void FontManager::TrueTypeFont::Char::setInfo(tagGlyphMetrics* metrics)
{
	memcpy(&_metrics, metrics, sizeof(tagGlyphMetrics));
}

void FontManager::TrueTypeFont::Char::getInfo(tagGlyphMetrics* metrics, TexturePtr& texture, float coords[])
{
	memcpy(metrics, &_metrics, sizeof(tagGlyphMetrics));
	texture = _texture;
	memcpy(coords, _coords, sizeof(float) * 4);
}

//------------------------------------------------------------------------
// Page
//------------------------------------------------------------------------
FontManager::TrueTypeFont::Page::Page()
{
	_wide = _tall = TRUE_TYPE_FONT_TEXTURE_SIZE;
	_posx = _posy = 0;

	// In a line, for a max height character
	_maxCharTall = 0;
	_texture = Root::getSingleton().getActiveRenderer()->createTexture(L"", NULL, true);
}

FontManager::TrueTypeFont::Page::~Page()
{
	// free the texture
}

bool FontManager::TrueTypeFont::Page::append(int wide, int tall, unsigned char* rgba, float coords[])
{
	if (_posy + tall > _tall)
	{
		// not enough line space in this page
		return false;
	}

	// If this line is full ...
	if (_posx + wide > _wide)
	{
		int newLineY = _posy + _maxCharTall;

		if (newLineY + tall > _tall)
		{
			// No more space for new line in this page, should allocate a new one
			return false;
		}

		// Begin in new line
		_posx = 0;
		_posy = newLineY;
		// Reset
		_maxCharTall = 0;
	}

	unsigned char* pData = (unsigned char*)_texture->lock(ECF_RGBA8888, Recti(_posx, _posy, _posx + wide, _posy + tall));
	memcpy(pData, rgba, wide*tall*4);
	_texture->unlock();

	coords[0] = _posx / (float)_wide;             // left
	coords[1] = _posy / (float)_tall;             // top
	coords[2] = (_posx + wide) / (float)_wide;    // right
	coords[3] = (_posy + tall) / (float)_tall;    // bottom

	_posx += wide;

	if (tall > _maxCharTall)
	{
		_maxCharTall = tall;
	}

	return true;
}

TexturePtr FontManager::TrueTypeFont::Page::getTexture()
{
	return _texture;
}

//------------------------------------------------------------------------
// FontManager
//------------------------------------------------------------------------
FontManager::FontManager()
{
	_library = NULL;
}

FontManager::~FontManager()
{
	release();
}

bool FontManager::initialize()
{
	FT_Error err;
	if ((err = FT_Init_FreeType(&_library)) != FT_Err_Ok)
	{
		Log::print(ELL_ERROR, "FT_Init_FreeType() Error %d", err);
		return false;
	}

	return true;
}

void FontManager::release()
{
	FT_Error err;
	for (size_t i = 0; i < _fonts.size(); i++)
	{
		delete _fonts[i];
		_fonts[i] = NULL;
	}

	_fonts.clear();
	if ((err = FT_Done_FreeType(_library)) != FT_Err_Ok)
	{
		Log::print(ELL_ERROR, "FT_Done_FreeType() Error %d", err);
	}
}

int FontManager::createFont(const char* filename, int face, int tall, bool bold, bool italic, bool antialias)
{
	TrueTypeFont *font = new TrueTypeFont();
	if (!font->create(_library, filename, face, tall, bold, italic, antialias))
	{
		delete font;
		return -1;
	}

	_fonts.push_back(font);
	return (int)_fonts.size() - 1;
}

bool FontManager::getCharInfo(int fontIndex, int code, int* wide, int* tall,
	int* bearingX, int* bearingY, int* advance, TexturePtr& texture, float coords[])
{
	int i = CONVERT_FONT_INDEX(fontIndex);
	if (i == -1)
		return false;

	TrueTypeFont *font = _fonts[i];
	tagGlyphMetrics metrics;

	if (!font->getCharInfo(code, &metrics, texture, coords))
		return false;

	*wide = metrics.Width;
	*tall = metrics.Height;
	*bearingX = metrics.BearingX;
	*bearingY = metrics.BearingY;
	*advance = metrics.Advance;

	return true;
}

int FontManager::getFontTall(int fontIndex)
{
	int i = CONVERT_FONT_INDEX(fontIndex);
	if (i == -1)
		return 0;

	TrueTypeFont* font = _fonts[i];
	return font->getFontTall();
}

NS_VAYO_END
