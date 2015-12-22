//
//  PlayFileCommand.cpp
//  mserver
//
//  Created by Yuval Dinari on 12/21/15.
//  Copyright © 2015 Vonage. All rights reserved.
//

#include "PlayFileCommand.hpp"
#include "AssignmentEvaluator.hpp"
#include "Connection.hpp"

//==========================================================================================================
//==========================================================================================================
void PlayFileCommand::interpret(string &line, ifstream &file, ScriptReader &reader)
{
    process_args(line, reader);
//    Connection connection;
//    connection.bind_socket(reader.get_value(AUDIO_IP), stoi(reader.get_value(AUDIO_PORT)), SOCK_DGRAM);
    
    
}


//==========================================================================================================
//==========================================================================================================
void PlayFileCommand::process_args(string& line, ScriptReader &reader)
{
    filename.clear();
    string file_opt = "file";
    
    map<string, string> vars;
    AssignmentEvaluator::inst()->eval(line, vars, reader);
    
    if(vars.count(file_opt) == 0)
    {
        throw string("No 'file' option to play file command");
    }
    else
    {
        filename = vars[file_opt];
    }
}
