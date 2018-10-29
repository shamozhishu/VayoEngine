#include "VayoNodeAnimator.h"
#include "VayoUtils.h"

NS_VAYO_BEGIN

bool NodeAnimator::hasFinished(void) const
{
	return false;
}

NodeAnimator::NodeAnimator(const wstring& name)
	: _name(name)
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

bool NodeAnimator::isInputEnabled() const
{
	return false;
}

void NodeAnimator::serialize(XMLElement* outXml)
{
	outXml->SetAttribute("name", unicodeToUtf8(_name).c_str());
}

bool NodeAnimator::deserialize(XMLElement* inXml)
{
	return inXml != NULL;
}

NS_VAYO_END
