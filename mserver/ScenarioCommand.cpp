
#include "ScenarioCommand.h"
#include "ScriptReader.h"
#include "mserver_utils.hpp"
#include "AssignmentEvaluator.hpp"


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
    scenario_file.clear();
    args.clear();
    string file_opt = "file";
    AssignmentEvaluator::inst()->eval(line, args, reader);

    if(args.count(file_opt) == 0)
    {
        throw string("No 'file' option to scenario command");
    }
    else
    {
        scenario_file = args[file_opt];
        args.erase(file_opt);
    }
}


