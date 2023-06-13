// Internal headers
#include "cli.hpp"
#include "enviroment_file.hpp"

//System headers
#include <string>
#include <optional>
#include <cstdlib>
#include <sstream>

using std::string;
using std::optional;
using std::ostringstream;

optional<string> get_enviroment_variable(string key) {
    
    char* value = getenv(key.c_str());

    if (value == NULL) return std::nullopt;
    return std::optional<string>{string(value)};

}

void set_enviroment_variable(string key, optional<string> value) {
    
    ostringstream fmt;
    fmt << key << "=" << value.value_or(string()) << std::ends;
    std::string text = fmt.str().c_str();

    char* output_variable = (char*) malloc(text.length() + 1);

    strcpy_s(output_variable,text.length() + 1,fmt.str().c_str());

    if (!putenv(output_variable)) {
        std::cerr << "ERROR: failed to write enviroment variable " << key << std::endl;
        exit(-1);
    }
}

int main(int argc, char* argv[]) {
    Cli args(argc, argv);
    EnviromentFile enviroment_file;
    
    enviroment_file.load(args.enviroment_file());
    for(EnviromentFile::iterator iterator = enviroment_file.begin(); iterator != enviroment_file.end(); iterator++) {
        string key = iterator->first;
        optional<string> value = iterator->second.empty() ? std::nullopt : optional<string>{iterator->second};
        
        set_enviroment_variable(key,value);
    }

    /**
     * WARNING Rant Incomming
     * 
     * This section here uses boost::process as it is the simplist way to spawn
     * a process without having to get in the weeds of understanding how this
     * is done on just about every single OS. however, boost is massive
     * basically a second std library which means that this causes to baloon in
     * size without wanting too. if C++ ever gets its shit together and adds
     * this functonality (like every other language in the past 20 years) then
     * boost can be removed and peace can be restored.
     * 
     */
}