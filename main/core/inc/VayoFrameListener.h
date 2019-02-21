/*************************************************************************\
* ÍûÓÑÒýÇæ1.0
* Copyright (c) 2018-2019 authored by Öì¼ÓºÆ
* Ö¡¼àÌýÆ÷
\*************************************************************************/
#ifndef __VAYO_FRAME_LISTENER_H__
#define __VAYO_FRAME_LISTENER_H__

#include "VayoCommon.h"
NS_VAYO_BEGIN

class FrameListener
{
public:
	virtual bool frameStarted(float dt) { (void)dt; return true; }
	virtual bool frameRendering(float dt) { (void)dt; return true; }
	virtual bool frameEnded(float dt) { (void)dt; return true; }
	virtual ~FrameListener() {}
};

NS_VAYO_END

#endif // __VAYO_FRAME_LISTENER_H__
