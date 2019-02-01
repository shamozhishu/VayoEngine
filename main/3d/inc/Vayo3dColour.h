/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 颜色封装类
\*************************************************************************/
#ifndef __VAYO3D_COLOUR_H__
#define __VAYO3D_COLOUR_H__

#include "VayoCommon.h"
NS_VAYO3D_BEGIN

enum EColorFormat
{
	ECF_GREY = 0,
	ECF_RGBA5551,
	ECF_BGRA5551,
	ECF_RGB565,
	ECF_BGR565,
	ECF_RGB888,
	ECF_BGR888,
	ECF_RGBA8888,
	ECF_BGRA8888,
	ECF_R16F,
	ECF_RG16F,
	ECF_RGBA16F,
	ECF_R32F,
	ECF_RG32F,
	ECF_RGBA32F,
	ECF_UNKNOWN
};

class _Vayo3dExport Colour
{
public:
	inline Colour()
		: _clr(0xff000000) {}
	inline Colour (int a, int r, int g, int b)
		: _clr(((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff))	{}
	inline Colour(unsigned int clr)
		: _clr(clr) {}

	inline int getAlpha() const	{	return (_clr>>24) & 0xff;	}
	inline int getRed() const	{	return (_clr>>16) & 0xff;	}
	inline int getGreen() const	{	return (_clr>>8) & 0xff;	}
	inline int getBlue() const	{	return _clr & 0xff;	        }

	inline void setAlpha(int a) {_clr = ((a & 0xff)<<24) | (((_clr>>16)& 0xff)<<16) | ((_clr>>8 & 0xff)<<8) | (_clr & 0xff);      }
	inline void setRed(int r)   {_clr = (((_clr>>24) & 0xff)<<24) | ((r & 0xff)<<16) | ((_clr>>8 & 0xff)<<8) | (_clr & 0xff);     }
	inline void setGreen(int g) {_clr = (((_clr>>24) & 0xff)<<24) | (((_clr>>16)& 0xff)<<16) | ((g & 0xff)<<8) | (_clr & 0xff);   }
	inline void setBlue(int b)  {_clr = (((_clr>>24) & 0xff)<<24) | (((_clr>>16)& 0xff)<<16) | ((_clr>>8 & 0xff)<<8) | (b & 0xff);}

	inline void set(int a, int r, int g, int b) { _clr = (((a & 0xff)<<24) | ((r & 0xff)<<16) | ((g & 0xff)<<8) | (b & 0xff)); }
	inline void set(unsigned int col) { _clr = col; }
	inline bool operator==(const Colour& other) const { return other._clr == _clr; }
	inline bool operator!=(const Colour& other) const { return other._clr != _clr; }

	inline Colour getInterpolated(Colour other, float d) const
	{
		float inv = 1.0f - d;
		return Colour((int)(other.getAlpha()*inv + getAlpha()*d),
			(int)(other.getRed()*inv + getRed()*d),
			(int)(other.getGreen()*inv + getGreen()*d),
			(int)(other.getBlue()*inv + getBlue()*d));
	}

	inline void toOpenGLColor(unsigned char* dest) const
	{
		*dest = (unsigned char)getRed();
		*++dest = (unsigned char)getGreen();
		*++dest = (unsigned char)getBlue();
		*++dest = (unsigned char)getAlpha();
	}

	// color in ECF_BGRA8888 Format
	unsigned int _clr;
};

NS_VAYO3D_END

#endif // __VAYO3D_COLOUR_H__
