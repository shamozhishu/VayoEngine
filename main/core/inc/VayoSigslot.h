/*************************************************************************\
* 望友引擎1.0
* Copyright (c) 2018-2019 authored by 朱加浩
* 信号-槽
\*************************************************************************/
#ifndef __VAYO_SIGSLOT_H__
#define __VAYO_SIGSLOT_H__

#include "VayoSupport.h"
#include "VayoSingleton.h"
#include "VayoUserDataBind.h"

#define signals public

NS_VAYO_BEGIN

class _VayoExport Signal
{
public:
	Signal();
	virtual ~Signal();
	UserDataBind& getUserDataBind() { return _userDataBind; }
	const UserDataBind& getUserDataBind() const { return _userDataBind; }
	PROPERTY_R(int, _sigid, SignalID)

private:
	friend class SignalTrigger;
	UserDataBind _userDataBind;
	mutable vector<class Slot*> _slotset;
	DISALLOW_COPY_AND_ASSIGN(Signal)
};

class Slot
{
public:
	virtual ~Slot() {}
	virtual bool execute(const Signal& sig) = 0;
};

class SlotFunction : public Slot
{
	friend class SignalTrigger;
public:
	typedef void (*SLOT_FUNCTION)(const Signal& sig);
	SlotFunction(SLOT_FUNCTION function)
		: _function(function) {}
	bool execute(const Signal& sig)
	{
		if (!_function)
			return false;
		_function(sig);
		return true;
	}

private:
	SLOT_FUNCTION _function;
};

template<typename T>
class SlotMethod : public Slot
{
	friend class SignalTrigger;
public:
	typedef void (T::*SLOT_METHOD)(const Signal& sig);
	SlotMethod(T* caller, SLOT_METHOD method)
		: _caller(caller), _method(method) {}
	bool execute(const Signal& sig)
	{
		if (!_caller || !_method)
			return false;
		(_caller->*(_method))(sig);
		return true;
	}

private:
	T*          _caller;
	SLOT_METHOD _method;
};

class _VayoExport SignalTrigger
{
public:
	static void emit(const Signal& sig);
	static void connect(const Signal& sig, SlotFunction::SLOT_FUNCTION function);
	static void disconnect(const Signal& sig, SlotFunction::SLOT_FUNCTION function);
	template<typename T> static void connect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD method);
	template<typename T> static void disconnect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD method);
	static void disconnect(const Signal& sig);
	static bool hasSignal(const Signal& sig);
};

template<typename T>
void SignalTrigger::connect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD method)
{
	VAYO_ASSERT(caller && method);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotMethod<T>* item = dynamic_cast<SlotMethod<T>*>(*it);
		if (NULL != item && caller == item->_caller && method == item->_method)
		{
			Log::wprint(ELL_WARNING, L"The slot don't support same method in same signal id %d!", sig.getSignalID());
			return; // 返回.
		}
	}
	sig._slotset.push_back(new SlotMethod<T>(caller, method));
}

template<typename T>
void SignalTrigger::disconnect(const Signal& sig, T* caller, typename SlotMethod<T>::SLOT_METHOD method)
{
	VAYO_ASSERT(caller && method);
	vector<Slot*>::const_iterator it = sig._slotset.cbegin();
	for (; it != sig._slotset.cend(); ++it)
	{
		SlotMethod<T>* item = dynamic_cast<SlotMethod<T>*>(*it);
		if (NULL != item && caller == item->_caller && method == item->_method)
		{
			SAFE_DELETE(item);
			sig._slotset.erase(it);
			break;
		}
	}
}

NS_VAYO_END

#endif // __VAYO_SIGSLOT_H__
