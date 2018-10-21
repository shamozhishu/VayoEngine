/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 应用程序基类
\*************************************************************************/
#ifndef __VAYO_APP_H__
#define __VAYO_APP_H__

#include "VayoInput.h"
#include "VayoRoot.h"

NS_VAYO_BEGIN

class _VayoExport App : public TouchDelegate, public KeypadDelegate
{
public:
	App(const Root::Config& config);
	virtual ~App();
	virtual void run();
	virtual bool startup() { return false; }
	virtual void cleanup() {}
};

NS_VAYO_END

#endif // __VAYO_APP_H__
