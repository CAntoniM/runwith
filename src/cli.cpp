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

Cli::Cli(int argc, char* argv[])  {
    bool verbose = false;
    bool env_set = false;
    int index = 1;

    for (; index < argc; index++) {
        bool at_end_of_args = index + 1 > argc;
        char enviroment_argument_short[] = "-e";
        char enviroment_argument_long[]  = "--env";
        bool is_enviroment_argument = 
            strcmp(argv[index],enviroment_argument_short) == 0 ||
            strcmp(argv[index],enviroment_argument_long) == 0;

        if ( is_enviroment_argument && !at_end_of_args && !env_set) {
            index ++;
            env_set = true;
            _enviroment_file_path = string(argv[index]);
            index ++;   
            continue;
        }

        char verbose_argument_short[] = "-v";
        char verbose_argument_long[]  = "--verbose";
        bool is_verbose_argument = 
            strcmp(argv[index],verbose_argument_short) == 0 ||
            strcmp(argv[index],verbose_argument_long) == 0;
        
        if (is_verbose_argument) {
            _verbose = true;
            continue;
        }

        break;
    }

    for (; index < argc; index++) {
        _command.push_back(string(argv[index]));
    }
}

#ifdef TEST_CLI
TEST_INIT(Cli)

TEST(no_args, testing the default behaviour if no args are given) {
    int argc = 1;
    char *argv[] = {"test", NULL};
    
    
    Cli cli(argc,argv);

    TEST_ASSERT(cli.command().size() == 0, Testing that the default values are set when no args are present and that they are arguments given);
    TEST_ASSERT(cli.verbose() == false, Testing that the default values are set when no args are present and that they are arguments given);
    TEST_ASSERT(cli.enviroment_file() == ".env" , Testing that the default values are set when no args are present and that they are arguments given);
}

TEST(verbose_arg, tesing the behaviour when a command that we are running shares arguments with us --verbose being a key example) {

    int argc = 2;
    char *argv[] = {"test","--verbose", NULL};
    
    Cli cli(argc,argv);
    
    TEST_ASSERT(cli.verbose() == true, Testing that the default verbose status is preserved);
    TEST_ASSERT(cli.enviroment_file() == ".env", Testing that the default enviroment file is preserved);
    TEST_ASSERT(cli.command().size() == 0, Testing that the default values are set when no args are present and that they are arguments given);
}

TEST(enviroment_file_arg, tesing the behaviour when a command that we are running shares arguments with us --verbose being a key example) {

    int argc = 3;
    char *argv[] = {"test","--env",".env", NULL};
    
    Cli cli(argc,argv);
    
    TEST_ASSERT(cli.verbose() == false, Testing that the default verbose status is preserved);
    TEST_ASSERT(cli.enviroment_file() == ".env", Testing that the default enviroment file is preserved);
    TEST_ASSERT(cli.command().size() == 0, Testing that the default values are set when no args are present and that they are arguments given);
}

TEST(overlapping_args, tesing the behaviour when a command that we are running shares arguments with us --verbose being a key example) {

    int argc = 3;
    char *argv[] = {"test","ls","--verbose", NULL};

    Cli cli(argc,argv);

    TEST_ASSERT(cli.verbose() == false, Testing that the default verbose status is preserved);
    TEST_ASSERT(cli.enviroment_file() == ".env", Testing that the default enviroment file is preserved);

    TEST_ASSERT(cli.command().size() == 2, Testing that right number of args are in the command);
    TEST_ASSERT(cli.command().at(0) == "ls", Testing that the fist arg is added to the commmands);
    TEST_ASSERT(cli.command().at(1) == "--verbose", Testing that the second arg is added to the commmands);
}

TEST(unknown_args, tesing the behaviour when uknown args are given) {

    int argc = 3;
    char *argv[] = {"test","ls","-al", NULL};

    Cli cli(argc,argv);
    
    TEST_ASSERT(cli.verbose() == false, Testing that the default verbose status is preserved);
    TEST_ASSERT(cli.enviroment_file() == ".env", Testing that the default enviroment file is preserved);

    TEST_ASSERT(cli.command().size() == 2, Testing that right number of args are in the command);
    TEST_ASSERT(cli.command().at(0) == "ls", Testing that the fist arg is added to the commmands);
    TEST_ASSERT(cli.command().at(1) == "-al", Testing that the second arg is added to the commmands);
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
