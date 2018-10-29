/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 对象属性序列化与反序列化
\*************************************************************************/
#ifndef __VAYO_ATTRIB_SERIALIZER_H__
#define __VAYO_ATTRIB_SERIALIZER_H__

#include "VayoSupport.h"
#include "VayoReflection.h"
#include "tinyxml2/tinyxml2.h"
using tinyxml2::XMLElement;

NS_VAYO_BEGIN

class _VayoExport AttribSerializer : public Type
{
public:
	virtual ~AttribSerializer() {}
	virtual void serialize(XMLElement* outXml) = 0;
	virtual bool deserialize(XMLElement* inXml) = 0;
	virtual bool parseCustomAttrib() { return true; }

protected:
	wstring _customAttribute;
};

NS_VAYO_END

#endif // __VAYO_ATTRIB_SERIALIZER_H__
