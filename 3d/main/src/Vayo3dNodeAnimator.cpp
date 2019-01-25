#include "Vayo3dNodeAnimator.h"
#include "Vayo3dSceneManager.h"
#include "VayoUtils.h"

NS_VAYO3D_BEGIN

bool NodeAnimator::hasFinished() const
{
	return false;
}

NodeAnimator::NodeAnimator(const wstring& name, SceneManager* originSceneMgr)
	: TouchDelegate(originSceneMgr->getName())
	, KeypadDelegate(originSceneMgr->getName())
	, _originSceneMgr(originSceneMgr)
	, _name(name)
{
	static unsigned short idx = 0;
	if (0 == _name.compare(L""))
	{
		std::wstringstream ss;
		ss << L"NodeAnimator" << idx;
		++idx;
		_name = ss.str();
	}
}

NodeAnimator::~NodeAnimator()
{
}

void NodeAnimator::serialize(XMLElement* outXml)
{
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());
}

bool NodeAnimator::deserialize(XMLElement* inXml)
{
	return inXml != NULL;
}

NS_VAYO3D_END
