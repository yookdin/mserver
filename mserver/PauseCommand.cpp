
// <pause[ for] num time-units/>

#include "PauseCommand.h"

//==========================================================================================================
// Parameter string of the command is for example:
// 1.5 seconds, 3 milliseconds, 1s, 1 second, 0.4 ms, etc.
//==========================================================================================================
PauseCommand::PauseCommand(): Command("pause"), params_regex("(\\d+(\\.\\d+)?) *(\\w+) *") {}


//==========================================================================================================
// Pause for the given amount of time. The amount and the units are converted to micro-seconds, and then
// usleep() is called.
//==========================================================================================================
void PauseCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    smatch match;
    
    if(!regex_search(line, match, params_regex))
    {
        throw string("Invalid parameters for pause command: " + line);
    }
    
    string pause_str = match[1].str() + " " + match[3].str();
    double n = stod(match[1]);
    n *= get_factor_for_units(match[3]); // To go from seconds/milliseconds to micro-seconds
    double integral_part;

    if(modf(n, &integral_part) != 0) // Check that no fractional part remains after factoring
    {
        throw string("Precision too high for pause command: " + pause_str + ". Maximum precision is micro-second.");
    }
    
    cout << "[" << get_time_string() << "] Pausing for " << pause_str << "..." << endl;
    usleep(n);
    cout << "[" << get_time_string() << "] Pause finished" << endl;
}


//==========================================================================================================
//==========================================================================================================
int PauseCommand::get_factor_for_units(string units)
{
    if(units == "s" || units == "second" || units == "seconds")
    {
        return 1000000;
    }

    if(units == "ms" || units == "millisecond" || units == "milliseconds")
    {
        return 1000;
    }
    
    throw string("Invalid format for pause command units: " + units);
}


//==========================================================================================================
//==========================================================================================================
string PauseCommand::get_time_string() {
    struct timeval  tv;
    struct  tm      *plocalTime;
    gettimeofday(&tv, NULL);
    plocalTime = localtime(&tv.tv_sec);
    int h       = plocalTime->tm_hour;
    int m       = plocalTime->tm_min;
    int s       = plocalTime->tm_sec;
    int milli   = tv.tv_usec / 1000;
    
    return to_string(h) + ":" + to_string(m) + ":" + to_string(s) + "." + to_string(milli);
}










