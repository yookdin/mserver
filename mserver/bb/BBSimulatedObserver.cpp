
#include "VxLog.h"
#include "BBSimulatedObserver.h"

using namespace std;

BBSimulatedObserver::BBSimulatedObserver(const std::string &s_state_name):
    _sStateName(s_state_name), _iState(0), _pUpdateMethod()
{
}

BBSimulatedObserver::~BBSimulatedObserver(void)
{
}

int BBSimulatedObserver::InternalStartObserving(IIntDelegatePtr p_update_method)
{
    LOG_DEBUG("%s:InternalStartObserving\n", _sStateName.c_str());
    _pUpdateMethod = p_update_method;
    return getValue();
}

void BBSimulatedObserver::InternalStopObserving()
{
    VOXIP_DLOG("%s:InternalStopObserving\n", _sStateName.c_str());
}

int BBSimulatedObserver::getValue()
{
    VOXIP_DLOG("%s:get_state Start\n", _sStateName.c_str());
    return _iState;
}

void BBSimulatedObserver::setValue(int state)
{
    VOXIP_DLOG("%s:set_value - called with [%d]\n", _sStateName.c_str(), state);
    _iState = state;

    if ( _pUpdateMethod )
    {
        _pUpdateMethod->Handle(_iState);
    }
}
