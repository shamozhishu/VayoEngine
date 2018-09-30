#include "VayoImage.h"
#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"
#include "VayoLog.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

static inline void convertRGBA8888ToBGRA8888(const void* sP, int sN, void* dP)
{
	unsigned char* sB = (unsigned char*)sP;
	unsigned char* dB = (unsigned char*)dP;

	for (int i = 0; i < sN; ++i)
	{
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];
		dB[3] = sB[3];
		sB += 4;
		dB += 4;
	}
}

static inline void convertBGRA8888ToRGBA8888(const void* sP, int sN, void* dP)
{
	convertRGBA8888ToBGRA8888(sP, sN, dP);
}

static inline void convertRGB888ToBGR888(const void* sP, int sN, void* dP)
{
	unsigned char* sB = (unsigned char*)sP;
	unsigned char* dB = (unsigned char*)dP;

	for (int i = 0; i < sN; ++i)
	{
		dB[0] = sB[2];
		dB[1] = sB[1];
		dB[2] = sB[0];
		sB += 3;
		dB += 3;
	}
}

static inline void convertBGR888ToRGB888(const void* sP, int sN, void* dP)
{
	convertRGB888ToBGR888(sP, sN, dP);
}

static inline void convertGreyToRed(const void* sP, int sN, void* dP)
{
	memcpy(dP, sP, sN);
}

static inline void convertRedToGrey(const void* sP, int sN, void* dP)
{
	convertGreyToRed(sP, sN, dP);
}

static inline int getBitsPerPixelFromFormat(EColorFormat format)
{
	int retVal = 0;
	switch (format)
	{
	case ECF_GREY:
		retVal = 8;
		break;
	case ECF_RGBA5551:
	case ECF_RGB565:
	case ECF_R16F:
		retVal = 16;
		break;
	case ECF_RGB888:
	case ECF_BGR888:
		retVal = 24;
		break;
	case ECF_RGBA8888:
	case ECF_BGRA8888:
	case ECF_RG16F:
	case ECF_R32F:
		retVal = 32;
		break;
	case ECF_RGBA16F:
	case ECF_RG32F:
		retVal = 64;
		break;
	case ECF_RGBA32F:
		retVal = 128;
		break;
	case ECF_UNKNOWN:
		break;
	default:
		break;
	}
	return retVal;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

Image::Image()
	: _data(NULL)
	, _bytesPerPixel(0)
	, _pitch(0)
	, _format(ECF_UNKNOWN)
{
}

Image::Image(EColorFormat format, const Dimension2di& size)
	: _data(NULL)
{
	reset(format, size);
}

Image::Image(const Image& img)
	: _data(NULL)
{
	*this = img;
}

Image::~Image()
{
	freeMemory();
}

Image& Image::operator=(const Image& img)
{
	if (this == &img)
		return *this;
	reset(img._format, img._size);
	if (_pitch == img._pitch)
		memcpy(_data, img._data, _size._height * _pitch);
	return *this;
}

bool Image::load(const wstring& filename)
{
	int width, height, nrComponents;
	unsigned char* srcData = stbi_load(w2a_(filename).c_str(), &width, &height, &nrComponents, 0);
	if (!srcData)
	{
		Log::wprint(ELL_ERROR, L"Texture failed to load at path: %s", filename.c_str());
		return false;
	}

	EColorFormat format = ECF_UNKNOWN;
	if (nrComponents == 1)
		format = ECF_GREY;
	else if (nrComponents == 3)
		format = ECF_BGR888;
	else if (nrComponents == 4)
		format = ECF_BGRA8888;

	reset(format, Dimension2di(width, height));
	void* dstData = lock();
	switch (format)
	{
	case ECF_GREY:
		convertGreyToRed(srcData, getImageDataSizeInPixels(), dstData);
		break;
	case ECF_BGR888:
		convertRGB888ToBGR888(srcData, getImageDataSizeInPixels(), dstData);
		break;
	case ECF_BGRA8888:
		convertRGBA8888ToBGRA8888(srcData, getImageDataSizeInPixels(), dstData);
		break;
	default:
		unlock();
		freeMemory();
		stbi_image_free(srcData);
		Log::wprint(ELL_ERROR, L"Texture failed to load at path: %s", filename.c_str());
		return false;
	}

	unlock();
	stbi_image_free(srcData);
	Log::wprint(ELL_INFORMATION, L"Texture succeed to load at path: %s", filename.c_str());
	return true;
}

void Image::save(const wstring& filename)
{
	unsigned char* pData = new unsigned char[_size._height * _pitch];
	switch (_format)
	{
	case ECF_GREY:
		convertRedToGrey(_data, getImageDataSizeInPixels(), pData);
		break;
	case ECF_BGR888:
		convertBGR888ToRGB888(_data, getImageDataSizeInPixels(), pData);
		break;
	case ECF_BGRA8888:
		convertBGRA8888ToRGBA8888(_data, getImageDataSizeInPixels(), pData);
		break;
	default:
		SAFE_DELETE_ARRAY(pData);
		Log::wprint(ELL_ERROR, L"Texture failed to save at path: %s", filename.c_str());
		return;
	}

	stbi_write_png(w2a_(filename).c_str(), _size._width, _size._height, getBytesPerPixel(), pData, 0);
	SAFE_DELETE_ARRAY(pData);
	Log::wprint(ELL_INFORMATION, L"Texture succeed to save at path: %s", filename.c_str());
}

void* Image::lock()
{
	return _data;
}

void Image::unlock()
{
}

void Image::reset(EColorFormat format, const Dimension2di& size)
{
	_format = format;
	_size = size;
	_bytesPerPixel = getBitsPerPixelFromFormat(_format) / 8u;
	_pitch = _bytesPerPixel * _size._width;
	freeMemory();
	_data = new unsigned char[_size._height * _pitch];
}

const Dimension2di& Image::getDimension() const
{
	return _size;
}

int Image::getBitsPerPixel() const
{
	return getBitsPerPixelFromFormat(_format);
}

int Image::getBytesPerPixel() const
{
	return _bytesPerPixel;
}

int Image::getImageDataSizeInBytes() const
{
	return _pitch * _size._height;
}

int Image::getImageDataSizeInPixels() const
{
	return _size._width * _size._height;
}

Colour Image::getPixel(unsigned int x, unsigned int y) const
{
	if (x >= _size._width || y >= _size._height)
		return Colour(0);

	unsigned char* p = NULL;
	switch (_format)
	{
	case ECF_GREY:
		p = _data + (y * _pitch) + x;
		return Colour(255, p[0], 0, 0);
	case ECF_BGR888:
		p = _data + (y * _pitch) + (x * 3);
		return Colour(255, p[2], p[1], p[0]);
	case ECF_BGRA8888:
		return ((unsigned int*)_data)[y*_size._width + x];
	}

	return Colour(0);
}

void Image::setPixel(unsigned int x, unsigned int y, const Colour& color)
{
	if (x >= _size._width || y >= _size._height)
		return;

	unsigned char* dest8 = NULL;
	unsigned int* dest32 = NULL;
	switch (_format)
	{
	case ECF_GREY:
		dest8 = _data + (y * _pitch) + x;
		dest8[0] = (unsigned char)color.getRed();
		break;
	case ECF_BGR888:
		dest8 = _data + (y * _pitch) + (x * 3);
		dest8[0] = (unsigned char)color.getBlue();
		dest8[1] = (unsigned char)color.getGreen();
		dest8[2] = (unsigned char)color.getRed();
		break;
	case ECF_BGRA8888:
		dest32 = (unsigned int*)(_data + (y * _pitch) + (x << 2));
		*dest32 = color._clr;
		break;
	}
}

EColorFormat Image::getColorFormat() const
{
	return _format;
}

int Image::getPitch() const
{
	return _pitch;
}

void Image::freeMemory()
{
	SAFE_DELETE_ARRAY(_data);
}

NS_VAYO_END
