/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2018 author by 朱加浩
* 用于设置状态位
\*************************************************************************/
#ifndef __VAYO_BIT_STATE_H__
#define __VAYO_BIT_STATE_H__

#include "VayoCommon.h"
NS_VAYO_BEGIN

enum
{
	ESTATE_00 = 0,
	ESTATE_01 = 1 << 0,
	ESTATE_02 = 1 << 1,
	ESTATE_03 = 1 << 2,
	ESTATE_04 = 1 << 3,
	ESTATE_05 = 1 << 4,
	ESTATE_06 = 1 << 5,
	ESTATE_07 = 1 << 6,
	ESTATE_08 = 1 << 7,
	ESTATE_09 = 1 << 8,
	ESTATE_10 = 1 << 9,

	EUI_EDIT_MODE_OPEN   = ESTATE_01,
	EUI_EDIT_MODE_SWITCH = ESTATE_02,
	EUI_EDIT_MODE_SAVE   = ESTATE_03,

	EDIRECTION_UPWARD    = ESTATE_01,
	EDIRECTION_DOWNWARD  = ESTATE_02,
	EDIRECTION_LEFTWARD  = ESTATE_03,
	EDIRECTION_RIGHTWARD = ESTATE_04,
	EDIRECTION_FORWARD   = ESTATE_05,
	EDIRECTION_BACKWARD  = ESTATE_06,

	ERECT_LT = ESTATE_01,
	ERECT_T  = ESTATE_02,
	ERECT_RT = ESTATE_03,
	ERECT_R  = ESTATE_04,
	ERECT_RB = ESTATE_05,
	ERECT_B  = ESTATE_06,
	ERECT_LB = ESTATE_07,
	ERECT_L  = ESTATE_08
};

class _VayoExport BitState
{
public:
	BitState() :_bitSet(0u) {}
	BitState(unsigned int state) :_bitSet(state) {}
	~BitState() {}

	unsigned int operator()() const
	{
		return _bitSet;
	}

	void operator()(unsigned int state)
	{
		_bitSet = state;
	}

	void addState(unsigned int state)
	{
		_bitSet |= state;
	}

	bool checkState(unsigned int state) const
	{
		return (_bitSet & state) == state;
	}

	void eraseState(unsigned int state)
	{
		_bitSet &= ~state;
	}

	bool isEmptyState() const
	{
		return 0u == _bitSet;
	}

	void clearState()
	{
		_bitSet = 0u;
	}

private:
	unsigned int _bitSet;
};

NS_VAYO_END

#endif // __VAYO_BIT_STATE_H__
