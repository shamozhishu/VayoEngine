/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* UI工厂
\*************************************************************************/
#ifndef __VAYO_UI_FACTORY_H__
#define __VAYO_UI_FACTORY_H__

#include "VayoSupport.h"
#include "VayoCoreGUI.h"
NS_VAYO_BEGIN

class UIFactory
{
public:
	UIFactory()
	{
		_ctrlLookupTable.insert(make_pair(L"Control", EUICT_UNKNOWN));
		_ctrlLookupTable.insert(make_pair(L"Button", EUICT_BUTTON));
		_ctrlLookupTable.insert(make_pair(L"CheckBox", EUICT_CHECK_BOX));
		_ctrlLookupTable.insert(make_pair(L"Static", EUICT_STATIC_TEXT));
		_ctrlLookupTable.insert(make_pair(L"ScrollBar", EUICT_SCROLL_BAR));
		_ctrlLookupTable.insert(make_pair(L"ListBox", EUICT_LIST_BOX));
		_ctrlLookupTable.insert(make_pair(L"ComboBox", EUICT_COMBO_BOX));
	}

	UIControl* createCtrl(const wstring& pCtrlName)
	{
		map<wstring, EUIControlType>::iterator it = _ctrlLookupTable.find(pCtrlName);
		if (it == _ctrlLookupTable.end())
			return NULL;
		switch (it->second)
		{
		case EUICT_UNKNOWN: return new UIControl();
		case EUICT_BUTTON: return new UIButton();
		case EUICT_CHECK_BOX: return new UICheckBox();
		case EUICT_STATIC_TEXT: return new UIStatic();
		case EUICT_SCROLL_BAR: return new UIScrollBar();
		case EUICT_LIST_BOX: return new UIListBox();
		case EUICT_COMBO_BOX: return new UIComboBox();
		default: break;
		}
		return NULL;
	}

private:
	map<wstring, EUIControlType> _ctrlLookupTable;
};

NS_VAYO_END

#endif // __VAYO_UI_FACTORY_H__
