
#pragma once

#include <string>
#include "VxObserverSubject.h"

class BBSimulatedObserver: public VxObserverSubject
{
public:
    BBSimulatedObserver(const std::string &s_state_name);
    virtual ~BBSimulatedObserver(void);

    virtual int	 getValue();
    virtual void setValue(int state);

protected:
    virtual int  InternalStartObserving(IIntDelegatePtr p_update_method);
    virtual void InternalStopObserving();

public:
    IIntDelegatePtr		_pUpdateMethod;
    std::string			_sStateName;
    int					_iState;
};
