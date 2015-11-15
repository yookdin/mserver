#include "ScenarioCommand.h"
#include "ScriptReader.h"

//==========================================================================================================
//==========================================================================================================
const regex ScenarioCommand::param_regex("file *= *\"(.*)\"");

//==========================================================================================================
//==========================================================================================================
void ScenarioCommand::interpret(string &line, ifstream &file)
{
	smatch match;
	
	if(! regex_search(line, match, param_regex))
	{
		throw string("Error in scenario command: \"" + line + "\"");
	}

    string scenario_file = match[1];
    
    try
    {
        ScriptReader nested_reader(scenario_file);
        reader.add_messages(nested_reader.get_messages());
    } catch (string err) {
        cout << err << endl;
        throw string("Error executing scenario file " + scenario_file);
    }
}