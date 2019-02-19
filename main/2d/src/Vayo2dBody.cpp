#include "Vayo2dBody.h"
#include "Vayo2dLayer.h"
#include "VayoUtils.h"
#include "Vayo2dGraphicQueue.h"

NS_VAYO2D_BEGIN

const wstring& Body::getName() const
{
	return _name;
}

Body::Body(const wstring& name, LayerManager* oriLayerMgr)
	: _name(name)
	, _parentLayer(NULL)
	, _visible(true)
	, _queueID(EZQ_MAIN_BODY)
	, _collideMask(0)
	, _oriLayerMgr(oriLayerMgr)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Body" << idx;
		++idx;
		_name = ss.str();
	}
}

Body::~Body()
{
	Layer* parent = _parentLayer;
	if (parent)
	{
		parent->detachBody(_name);
	}
}

Layer* Body::getParentLayer() const
{
	return _parentLayer;
}

bool Body::isVisible() const
{
	return _visible;
}

void Body::setVisible(bool visible)
{
	_visible = visible;
}

Rectf Body::getWorldArea() const
{
	Rectf worldArea = getLocalArea();
	if (_parentLayer && worldArea.isValid())
		_parentLayer->getAbsTransform().transformArea(worldArea);
	return worldArea;
}

const Rectf& Body::getLocalArea() const
{
	return _localArea;
}

void Body::refreshLocalArea()
{

}

BitState& Body::getCollideMask()
{
	return _collideMask;
}

UserDataBind& Body::getUserDataBind()
{
	return _userDataBind;
}

const UserDataBind& Body::getUserDataBind() const
{
	return _userDataBind;
}

void Body::serialize(XMLElement* outXml)
{
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());
	if (!_visible)
		outXml->SetAttribute("visible", _visible);
	if (_queueID != EZQ_MAIN_BODY)
		outXml->SetAttribute("queueid", _queueID);
	if (!_collideMask.isEmptyState())
		outXml->SetAttribute("collideMask", _collideMask());

	wstringstream ss;
	ss << _userDataBind;
	string strUserData = unicodeToUtf8(ss.str());
	if (!strUserData.empty())
	{
		outXml->LinkEndChild(outXml->GetDocument()->NewText(strUserData.c_str()));
	}
}

bool Body::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;
	inXml->QueryBoolAttribute("visible", &_visible);
	inXml->QueryUnsignedAttribute("queueid", &_queueID);
	_collideMask(inXml->UnsignedAttribute("collideMask"));

	const char* szUserData = inXml->GetText();
	if (szUserData)
	{
		wstringstream ss;
		ss << utf8ToUnicode(szUserData);
		ss >> _userDataBind;
	}

	return true;
}

NS_VAYO2D_END
