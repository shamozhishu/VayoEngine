#include "Vayo2dLayer.h"
#include "Vayo2dBody.h"
#include "VayoUtils.h"
#include "Vayo2dLayerManager.h"
#include "Vayo2dWireBoundingRect.h"

NS_VAYO2D_BEGIN

Reflex<Layer, const wstring&, Joint*, LayerManager*> Layer::_dynReflex;
Layer::Layer(const wstring& name, Joint* parent, LayerManager* oriLayerMgr)
	: Joint(name, parent, oriLayerMgr)
	, _wireBoundingRect(nullptr)
	, _showBoundingRect(false)
	, _isAutomaticCulling(true)
{
	static unsigned long long idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Layer" << idx;
		++idx;
		_name = ss.str();
	}
	_wireBoundingRect = new WireBoundingRect();
}

Layer::~Layer()
{
	SAFE_DELETE(_wireBoundingRect);
	detachAllBodies();
}

void Layer::visit(float dt)
{
	if (isCanVisit())
	{
		updateLocalRect();
		animating(dt);
		updateWorldRect();
		if (!_oriLayerMgr->isCulled(this))
		{
			if (_showBoundingRect)
				_oriLayerMgr->registerForRendering(_wireBoundingRect, EGQ_WIRE_BOUNDING_RECT);

			Body* pBody;
			map<wstring, Body*>::iterator it = _bodies.begin();
			for (; it != _bodies.end(); ++it)
			{
				pBody = it->second;
				if (pBody->isVisible())
					pBody->update(dt);
			}
		}
	}
	Joint::visit(dt);
}

void Layer::showWireBoundingRect(bool bShow)
{
	_showBoundingRect = bShow;
}

bool Layer::isShowWireBoundingRect() const
{
	if (nullptr == _wireBoundingRect)
		return false;
	return _showBoundingRect;
}

const Rectf& Layer::getWorldRect() const
{
	return _worldRect;
}

void Layer::updateWorldRect()
{
	_worldRect = Rectf(1, 1, 0, 0);
	map<wstring, Body*>::iterator it = _bodies.begin();
	for (; it != _bodies.end(); ++it)
	{
		_worldRect.addInternalRect(it->second->getWorldRect());
	}
	((WireBoundingRect*)_wireBoundingRect)->setRect(_worldRect);
}

void Layer::updateLocalRect()
{
	map<wstring, Body*>::iterator it = _bodies.begin();
	for (; it != _bodies.end(); ++it)
	{
		it->second->refreshLocalRect();
	}
}

void Layer::attachBody(Body* body)
{
	if (!body || body->getOriLayerMgr() != getOriLayerMgr())
		return;
	if (nullptr == getAttacheBody(body->getName()))
	{
		_bodies.insert(make_pair(body->getName(), body));
		if (body->_parentLayer)
			body->_parentLayer->detachBody(body->getName());
		body->_parentLayer = this;
	}
}

int Layer::getAttachedBodiesCount() const
{
	return (int)_bodies.size();
}

Body* Layer::getAttacheBody(const wstring& name)
{
	Body* ret = nullptr;
	map<wstring, Body*>::iterator it = _bodies.find(name);
	if (it != _bodies.end())
		ret = it->second;
	return ret;
}

void Layer::detachBody(Body* body)
{
	if (body)
		detachBody(body->getName());
}

Body* Layer::detachBody(const wstring& name)
{
	Body* ret = nullptr;
	map<wstring, Body*>::iterator it = _bodies.find(name);
	if (it != _bodies.end())
	{
		ret = it->second;
		ret->_parentLayer = nullptr;
		_bodies.erase(it);
	}
	return ret;
}

void Layer::detachAllBodies()
{
	map<wstring, Body*>::iterator it = _bodies.begin();
	for (; it != _bodies.end(); ++it)
	{
		it->second->_parentLayer = nullptr;
	}
	_bodies.clear();
}

Layer* Layer::createChildLayer(const wstring& name /*= L""*/)
{
	Layer* pRet = nullptr;
	if (_oriLayerMgr)
		pRet = _oriLayerMgr->createLayer<Layer>(this, name);
	return pRet;
}

void Layer::serialize(XMLElement* outXml)
{
	char szbuf[256];
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());

	if (_position != Vector2df::Origin)
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f,%f", _position._x, _position._y);
		outXml->SetAttribute("position", szbuf);
	}

	if (!isZero(_rotAngle))
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f", _rotAngle);
		outXml->SetAttribute("rotangle", szbuf);
	}

	if (_scale != Vector2df(1, 1))
	{
		sprintf_s(szbuf, sizeof(szbuf), "%f,%f", _scale._x, _scale._y);
		outXml->SetAttribute("scale", szbuf);
	}

	if (!_canVisit)
		outXml->SetAttribute("visit", _canVisit);

	if (_showBoundingRect)
		outXml->SetAttribute("show_boundingrect", _showBoundingRect);

	if (!_isAutomaticCulling)
		outXml->SetAttribute("auto_culling", _isAutomaticCulling);

	wstringstream ss;
	ss << _userDataBind;
	string strUserData = unicodeToUtf8(ss.str());
	if (!strUserData.empty())
	{
		outXml->LinkEndChild(outXml->GetDocument()->NewText(strUserData.c_str()));
	}
}

bool Layer::deserialize(XMLElement* inXml)
{
	if (!inXml)
		return false;

	string strTemp;
	vector<string> container;
	const char* szTmp = inXml->Attribute("position");
	if (szTmp)
		strTemp = szTmp;
	stringtok(container, strTemp, ",");
	if (container.size() >= 2)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		_position.set(x, y);
	}

	_rotAngle = inXml->FloatAttribute("rotangle");

	strTemp.clear();
	container.clear();
	szTmp = inXml->Attribute("scale");
	if (szTmp)
		strTemp = szTmp;
	stringtok(container, strTemp, ",");
	if (container.size() >= 2)
	{
		float x = (float)atof(container[0].c_str());
		float y = (float)atof(container[1].c_str());
		_scale.set(x, y);
	}

	inXml->QueryBoolAttribute("visit", &_canVisit);
	inXml->QueryBoolAttribute("show_boundingrect", &_showBoundingRect);
	inXml->QueryBoolAttribute("auto_culling", &_isAutomaticCulling);

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
