#include "VayoSigslot.h"
#include "VayoLog.h"

NS_VAYO_BEGIN

bool SignalTrigger::hasSignal(const Signal& sig)
{
	return !sig._slotset.empty();
}

Signal::Signal()
{
	static int idx = 0;
	_sigid = idx++;
}

Signal::~Signal()
{
	SignalTrigger::disconnect(*this);
}

void SignalTrigger::emit(const Signal& sig)
{
	vector<Slot*>::const_iterator itSlot = sig._slotset.cbegin();
	for (; itSlot != sig._slotset.cend(); ++itSlot)
	{
		Slot* const &item = *itSlot;
		if (item)
			item->execute(sig);
	}
}

void SignalTrigger::connect(const Signal& sig, SlotFunction::SLOT_FUNCTION function)
{
	VAYO_ASSERT(function);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && function == item->_function)
		{
			Log::wprint(ELL_WARNING, L"The slot don't support same function in same signal id %d!", sig.getSignalID());
			return; // ·µ»Ø.
		}
	}
	sig._slotset.push_back(new SlotFunction(function));
}

void SignalTrigger::disconnect(const Signal& sig, SlotFunction::SLOT_FUNCTION function)
{
	VAYO_ASSERT(function);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotFunction* item = dynamic_cast<SlotFunction*>(*it);
		if (NULL != item && function == item->_function)
		{
			SAFE_DELETE(item);
			sig._slotset.erase(it);
			break;
		}
	}
}

void SignalTrigger::disconnect(const Signal& sig)
{
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
		delete *it;
	sig._slotset.clear();
}

NS_VAYO_END
