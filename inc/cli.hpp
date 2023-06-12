#ifndef _CLI_H_
#define _CLI_H_

#include <string>
#include <vector>

using std::string;
using std::vector;

/**
 * @brief A wrapper for parsing the commandline arguments
 * 
 * This object takes in the commandline arguments and parses them into a format
 * that the programme is much more looking for and then gives you an api for
 * getting those arguments.
 * 
 */
class Cli {
    private:
        string _enviroment_file_path;
        bool _verbose = false;
        vector<string> _command;
    public:

        /**
         * @brief Construct a new Cli object by parsing the commandline args 
         * given from main.
         * 
         * @param argc 
         * @param argv 
         */
        Cli(int argc, char* argv[]);

        /**
         * @brief reads the enviroment file from disk and returns its contents
         * 
         * @return std::string
         */
        string enviroment_file();

        /**
         * @brief returns if the programme should be doing verbose logging
         * 
         */
        bool verbose();

        /**
         * @brief returns the command that the user wishes to run.
         * 
         * @return vector<string> 
         */
        vector<string> command();
};

#endif
