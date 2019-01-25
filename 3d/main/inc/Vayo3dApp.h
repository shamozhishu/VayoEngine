/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2019 authored by ��Ӻ�
* Ӧ�ó������
\*************************************************************************/
#ifndef __VAYO3D_APP_H__
#define __VAYO3D_APP_H__

#include "VayoInput.h"
#include "Vayo3dRoot.h"

NS_VAYO3D_BEGIN

class _Vayo3dExport App : public TouchDelegate, public KeypadDelegate
{
public:
	App(const Root::Config& config);
	virtual ~App();
	virtual void run();
	virtual bool startup() { return false; }
	virtual void cleanup() {}
};

NS_VAYO3D_END

#endif // __VAYO3D_APP_H__
