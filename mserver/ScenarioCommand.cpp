
#include "ScenarioCommand.h"
#include "ScriptReader.h"
#include "mserver_utils.hpp"


//==========================================================================================================
//==========================================================================================================
void ScenarioCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line);
    
    try
    {
        ScriptReader nested_reader(scenario_file, args, &reader);
        reader.add_messages(nested_reader.get_messages());
    } catch (string err) {
        cout << err << endl;
        throw string("Error executing scenario file " + scenario_file);
    }
    
    reader.print_continue_title();
}


//==========================================================================================================
//==========================================================================================================
void ScenarioCommand::process_args(string& line)
{
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
