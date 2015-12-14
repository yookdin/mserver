
#include "ScenarioCommand.h"
#include "ScriptReader.h"
#include "mserver_utils.hpp"


//==========================================================================================================
//==========================================================================================================
void ScenarioCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line, reader);
    
    try
    {
        ScriptReader nested_reader(scenario_file, args, &reader);
    } catch (string err) {
        // Roll the exception up to be caught at top level
        throw string("Error executing scenario file " + scenario_file + ":\n    " + err);
    }
    
    reader.print_continue_title();
}


//==========================================================================================================
//==========================================================================================================
void ScenarioCommand::process_args(string& line, ScriptReader &reader)
{
    reader.replace_vars(line);
    scenario_file.clear();
    args.clear();
    map<string, Option> options;
    options.emplace("file", Option(true, true));
    options.emplace(DEFAULT_PV_NAME, ParamValOption()); // Will match any var=val and put it as a new option in the map
    
    OptionParser parser(line, options, '>');
    
    for(auto pair: options)
    {
        if(pair.first == "file")
        {
            scenario_file = pair.second.get_value();
        }
        else
        {
            args[pair.first] = pair.second.get_value();
        }
    }
}


//==========================================================================================================
//==========================================================================================================
string ScenarioCommand::get_start_regex_str()
{
    return "<(scenario)";
}
