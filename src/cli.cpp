#ifdef TEST_CLI
#include "test_utils.hpp"
#endif
// Internal headers
#include "cli.hpp"

// System headers
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <iostream>
#include <fstream>

// using statement
using std::string;
using std::vector;
using std::ifstream;

#ifdef TEST_CLI
TEST_INIT(Cli)
#endif

Cli::Cli(int argc, char* argv[])  {
    string enviroment_file_path;
    bool verbose = false;
    int command_index = 0;

    for (int index = 0 ; index < argc; index++) {

        bool at_end_of_args = index + 1 < argc;
        char enviroment_argument_short[] = "-e";
        char enviroment_argument_long[]  = "--env";
        bool is_enviroment_argument = 
            strcmp(argv[index],enviroment_argument_short) == 0 ||
            strcmp(argv[index],enviroment_argument_long) == 0;
        
        if ( is_enviroment_argument && !at_end_of_args && enviroment_file_path.empty()) {
            index ++;
            enviroment_file_path = string(argv[index]);
            continue;
        }

        char verbose_argument_short[] = "-v";
        char verbose_argument_long[]  = "--verbose";
        bool is_verbose_argument = 
            strcmp(argv[index],verbose_argument_short) == 0 ||
            strcmp(argv[index],verbose_argument_long) == 0;
        
        if (is_verbose_argument) {
            verbose = true;
            continue;
        }

        command_index = index;
        break;
    }

    vector<string> command;

    for (int index = command_index; index < argc; index++) {
        command.push_back(string(argv[index]));
    }

    _enviroment_file_path = enviroment_file_path;
    _verbose = verbose;
    _command = command;
}

#ifdef TEST_CLI
TEST(constructor, this is the main test of the Cli parsing and retrival of its results) {
    
}
#endif


string Cli::enviroment_file() {
    return _enviroment_file_path;
}

bool Cli::verbose() {
    return _verbose;
}

vector<string> Cli::command() {
    return _command;
}
