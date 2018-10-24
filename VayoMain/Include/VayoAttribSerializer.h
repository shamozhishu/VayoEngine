/*************************************************************************\
* ��������1.0
* Copyright (c) 2018-2018 author by ��Ӻ�
* �����������л��뷴���л�
\*************************************************************************/
#ifndef __VAYO_ATTRIB_SERIALIZER_H__
#define __VAYO_ATTRIB_SERIALIZER_H__

#include "VayoCommon.h"
#include "tinyxml2/tinyxml2.h"
using tinyxml2::XMLElement;

NS_VAYO_BEGIN

class _VayoExport AttribSerializer
{
public:
	virtual ~AttribSerializer() {}
	virtual void serialize(XMLElement* outXml) = 0;
	virtual bool deserialize(XMLElement* inXml) = 0;
};

NS_VAYO_END

#endif // __VAYO_ATTRIB_SERIALIZER_H__
