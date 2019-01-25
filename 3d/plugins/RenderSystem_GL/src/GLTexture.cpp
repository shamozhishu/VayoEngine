#include "GLTexture.h"
#include "VayoLog.h"
#include "Vayo3dImage.h"
#include "GLRenderSystem.h"

void* GLTexture::lock(EColorFormat fmt, Recti rc)
{
	_colorFormat = fmt;
	_uploadArea = rc;
	_imageData->reset(_colorFormat, _uploadArea.getSize());
	return _imageData->lock();
}

GLTexture::GLTexture(const wstring& name, Image* image, bool generateMipLevels, GLRenderSystem* renderSys)
	: Texture(name)
	, _imageData(NULL)
	, _colorFormat(ECF_RGBA8888)
	, _pixelFormat(GL_RGBA)
	, _pixelType(GL_UNSIGNED_BYTE)
	, _internalFormat(GL_RGBA)
	, _textureName(0)
	, _hasMipMaps(generateMipLevels)
	, _pitch(0)
	, _textureSize(256, 256)
	, _imageSize(256, 256)
	, _uploadArea(0, 0, 0, 0)
	, _renderSystem(renderSys)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"OpenGL_Texture" << idx;
		++idx;
		_name = ss.str();
	}

	glGenTextures(1, &_textureName);

	if (image)
	{
		getImageAttrib(image);
		_imageData = new Image(_colorFormat, _textureSize);
		getImageData(image);
		uploadTexture(true);
	}
	else
	{
		_imageData = new Image();
		glBindTexture(GL_TEXTURE_2D, _textureName);
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _textureSize._width,
			_textureSize._height, 0, _pixelFormat, _pixelType, NULL);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (_hasMipMaps)
		{
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
		}
	}
}

GLTexture::~GLTexture()
{
	SAFE_DELETE(_imageData);
	glDeleteTextures(1, &_textureName);
}

void GLTexture::unlock()
{
	_imageData->unlock();
	uploadTexture(false);
}

const Dimension2di& GLTexture::getImgSize() const
{
	return _imageSize;
}

const Dimension2di& GLTexture::getTexSize() const
{
	return _textureSize;
}

EColorFormat GLTexture::getColorFormat() const
{
	return _colorFormat;
}

ERenderSystemType GLTexture::getRenderSystemType() const
{
	return ERST_OPENGL;
}

int GLTexture::getPitch() const
{
	if (_imageData)
		return _imageData->getPitch();
	return 0;
}

bool GLTexture::hasMipMaps() const
{
	return _hasMipMaps;
}

GLuint GLTexture::getOpenGLTextureName() const
{
	return _textureName;
}

void GLTexture::getImageAttrib(Image* image)
{
	if (!image)
	{
		Log::wprint(ELL_ERROR, L"No image for OpenGL texture.");
		return;
	}

	_colorFormat = image->getColorFormat();
	_imageSize = image->getDimension();

	if (_imageSize._width <= 0 || _imageSize._height <= 0)
	{
		return;
	}

	const float ratio = (float)_imageSize._width / (float)_imageSize._height;
	if ((_imageSize._width > _renderSystem->getMaxTextureSize()) && (ratio >= 1.0f))
	{
		_imageSize._width = _renderSystem->getMaxTextureSize();
		_imageSize._height = (int)(_renderSystem->getMaxTextureSize() / ratio);
	}
	else if (_imageSize._height > _renderSystem->getMaxTextureSize())
	{
		_imageSize._height = _renderSystem->getMaxTextureSize();
		_imageSize._width = (int)(_renderSystem->getMaxTextureSize() * ratio);
	}

	_textureSize._width = getTextureSizeFromImageSize(_imageSize._width);
	_textureSize._height = getTextureSizeFromImageSize(_imageSize._height);
}

void GLTexture::getImageData(Image* image)
{
	if (_textureSize._width <= 0 || _textureSize._height <= 0 ||
		_imageSize._width <= 0 || _imageSize._height <= 0)
	{
		Log::wprint(ELL_ERROR, L"Could not create OpenGL Texture.");
		return;
	}

	if (_textureSize == _imageSize)
	{
		*_imageData = *image;
	}
	else
	{
		// scale texture
		float sourceXStep = (float)_imageSize._width / _textureSize._width;
		float sourceYStep = (float)_imageSize._height / _textureSize._height;
		float sy;
		int bytesPerPixel = image->getBytesPerPixel();

		unsigned char* dst = (unsigned char*)_imageData->lock();
		unsigned char* src = (unsigned char*)image->lock();

		// copy texture scaling
		for (int x = 0; x < _textureSize._width; ++x)
		{
			sy = 0.0f;

			for (int y = 0; y < _textureSize._height; ++y)
			{
				memcpy(dst, src, bytesPerPixel);
				dst += (y * _textureSize._width + x) * bytesPerPixel;
				src += (int)(((int)sy)*_imageSize._width + x*sourceXStep) * bytesPerPixel;
				sy += sourceYStep;
			}
		}

		image->unlock();
		_imageData->unlock();
	}
}

void GLTexture::uploadTexture(bool newTexture)
{
	if (!_imageData)
	{
		Log::wprint(ELL_ERROR, L"No image for OpenGL texture to upload");
		return;
	}

	GLenum oldInternalFormat = _internalFormat;
	GLint filtering;
	_internalFormat = getGLFormatAndParametersFromColorFormat(_colorFormat, filtering, _pixelFormat, _pixelType);
	// make sure we don't change the internal format of existing images
	if (!newTexture)
		_internalFormat = oldInternalFormat;

	_renderSystem->setActiveTexture(0, this);

	if (newTexture)
	{
		if (_hasMipMaps)
		{
			glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
			// enable bilinear mipmap filter
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
		else
		{
			// enable bilinear filter without mipmaps
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	void* sourceData = _imageData->lock();
	if (newTexture)
	{
		_renderSystem->testGLErrorBegan();
		glTexImage2D(GL_TEXTURE_2D, 0, _internalFormat, _textureSize._width,
			_textureSize._height, 0, _pixelFormat, _pixelType, sourceData);
		_renderSystem->testGLErrorEnded(L"Could not glTexImage2D");
	}
	else
	{
		_renderSystem->testGLErrorBegan();
		glTexSubImage2D(GL_TEXTURE_2D, 0, _uploadArea._upperLeftCorner._x, _uploadArea._upperLeftCorner._y,
			_uploadArea.getWidth(), _uploadArea.getHeight(), _pixelFormat, _pixelType, sourceData);
		_renderSystem->testGLErrorEnded(L"Could not glTexSubImage2D");
	}
	_imageData->unlock();

	if (_hasMipMaps)
		glGenerateMipmap(GL_TEXTURE_2D);
}

inline int GLTexture::getTextureSizeFromImageSize(int size)
{
	int texOptimizeSize = 0x01;
	while (texOptimizeSize < size)
		texOptimizeSize <<= 1;
	return texOptimizeSize;
}

GLint GLTexture::getGLFormatAndParametersFromColorFormat(EColorFormat colorfmt,
	GLint& filtering, GLenum& pixelFmt, GLenum& pixelType)
{
	filtering = GL_LINEAR;
	pixelFmt = GL_RGBA;
	pixelType = GL_UNSIGNED_BYTE;
	GLint internalformat = GL_RGBA;

	switch (colorfmt)
	{
	case ECF_GREY:
		pixelFmt = GL_RED;
		internalformat = GL_RED;
		break;
	case ECF_RGBA5551:
		pixelType = GL_UNSIGNED_SHORT_5_5_5_1;
		break;
	case ECF_RGB565:
		pixelFmt = GL_RGB;
		pixelType = GL_UNSIGNED_SHORT_5_6_5;
		internalformat = GL_RGB;
		break;
	case ECF_RGB888:
		pixelFmt = GL_RGB;
		internalformat = GL_RGB;
		break;
	case ECF_BGR888:
		pixelFmt = GL_BGR;
		internalformat = GL_RGB;
		break;
	case ECF_RGBA8888:
		break;
	case ECF_BGRA8888:
		pixelFmt = GL_BGRA;
		break;
		// Floating Point texture formats.
	case ECF_R16F:
		filtering = GL_NEAREST;
		pixelFmt = GL_RED;
		pixelType = GL_FLOAT;
		internalformat = GL_R16F;
		break;
	case ECF_RG16F:
		filtering = GL_NEAREST;
		pixelFmt = GL_RG;
		pixelType = GL_FLOAT;
		internalformat = GL_RG16F;
		break;
	case ECF_RGBA16F:
		filtering = GL_NEAREST;
		pixelType = GL_FLOAT;
		internalformat = GL_RGBA16F;
		break;
	case ECF_R32F:
		filtering = GL_NEAREST;
		pixelFmt = GL_RED;
		pixelType = GL_FLOAT;
		internalformat = GL_R32F;
		break;
	case ECF_RG32F:
		filtering = GL_NEAREST;
		pixelFmt = GL_RG;
		pixelType = GL_FLOAT;
		internalformat = GL_RG32F;
		break;
	case ECF_RGBA32F:
		filtering = GL_NEAREST;
		pixelType = GL_FLOAT;
		internalformat = GL_RGBA32F;
		break;
	default:
		Log::wprint(ELL_ERROR, L"Unsupported texture format");
		internalformat = GL_RGBA8;
		break;
	}

	return internalformat;
}
