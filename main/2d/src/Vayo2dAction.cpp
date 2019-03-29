#include "Vayo2dAction.h"
#include "Vayo2dLayerManager.h"
#include "VayoUtils.h"

NS_VAYO2D_BEGIN

bool Action::hasFinished() const
{
	return false;
}

Action::Action(const wstring& name, LayerManager* oriLayerMgr)
	: TouchDelegate(oriLayerMgr->getName())
	, KeypadDelegate(oriLayerMgr->getName())
	, _oriLayerMgr(oriLayerMgr)
	, _name(name)
{
	static unsigned long long idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"Action" << idx;
		++idx;
		_name = ss.str();
	}
}

Action::~Action()
{
}

void Action::serialize(XMLElement* outXml)
{
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());
}

bool Action::deserialize(XMLElement* inXml)
{
	return inXml != NULL;
}

NS_VAYO2D_END
