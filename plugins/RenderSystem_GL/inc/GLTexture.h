/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* OpenGL纹理
\*************************************************************************/
#ifndef __GL_TEXTURE_H__
#define __GL_TEXTURE_H__

#include "GLSupport.h"
#include "Vayo3dTexture.h"

class GLTexture : public Texture
{
public:
	GLTexture(const wstring& filename, Image* image, bool generateMipLevels, GLRenderSystem* renderSys);
	~GLTexture();
	void*               lock(EColorFormat fmt, Recti rc);
	void                unlock();
	const Dimension2di& getImgSize() const;
	const Dimension2di& getTexSize() const;
	EColorFormat        getColorFormat() const;
	ERenderSystemType   getRenderSystemType() const;
	int                 getPitch() const;
	bool                hasMipMaps() const;
	GLuint              getOpenGLTextureName() const;

protected:
	void  getImageAttrib(Image* image);
	void  getImageData(Image* image);
	void  uploadTexture(bool newTexture);
	int   getTextureSizeFromImageSize(int size);
	GLint getGLFormatAndParametersFromColorFormat(EColorFormat colorfmt, GLint& filtering, GLenum& pixelFmt, GLenum& pixelType);

protected:
	Image*          _imageData;
	EColorFormat    _colorFormat;
	GLenum          _pixelFormat;
	GLenum          _pixelType;
	GLint           _internalFormat;
	GLuint          _textureName;
	bool            _hasMipMaps;
	int             _pitch;
	Dimension2di    _textureSize;
	Dimension2di    _imageSize;
	Recti           _uploadArea;
	GLRenderSystem* _renderSystem;
};

#endif // __GL_TEXTURE_H__
