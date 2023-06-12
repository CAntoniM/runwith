
#ifdef TEST_ENVIROMENT_FILE
#include "test_utils.hpp"
#endif
#include "enviroment_file.hpp"

#include <sstream>
#include <optional>

using std::map;
using std::ifstream;
using std::ofstream;
using std::string;
using std::optional;

#ifdef TEST_ENVIROMENT_FILE
TEST_INIT();
#endif

EnviromentFile::EnviromentFile(string file_name){
    this->file_name(file_name);
}

#ifdef TEST_ENVIROMENT_FILE
TEST(EnviromentFile_constructor, a test to ensure the default constructor works) {
    TEST_ASSERT(true,Figuring out if this works at all);
}
#endif

void EnviromentFile::file_name(string file_name){
    _file_name = file_name;
}

string EnviromentFile::file_name(){
    return _file_name;
}

void EnviromentFile::read(){
    ifstream file;
    file.exceptions(ifstream::badbit);

    try {

        file.open(_file_name);
        string line;
        
        while (std::getline(file,line))
        {
            size_t split_posistion = line.find('=');

            if (split_posistion == string::npos) {
                std::cerr << "ERROR: " << _file_name << "is in a invalid file format" << std::endl;
                exit(-1);
            }

            string key = line.substr(0,split_posistion);
            string value = line.substr(split_posistion+1);
            _data.insert(std::make_pair(key,value));

        } 
    } catch(const ifstream::failure& e) {

        std::cerr << "ERROR: " << e.what() << std::endl;
        exit(-1);

    }
}

void EnviromentFile::write(){
    ofstream file;
    file.exceptions(ofstream::badbit);

    try {
        file.open(_file_name);

        for( EnviromentFile::iterator iterator = begin(); iterator != end(); iterator++) {
            file << iterator->first << " = " << iterator->second << std::endl;
        }
    } catch (const ofstream::failure& e ) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        exit(-1); 
    }
}

optional<std::string> EnviromentFile::get(std::string key){

    EnviromentFile::iterator position = _data.find(key);

    if (position == _data.end()) {
        return std::nullopt;
    }

    return optional<string>{position->second};
}

void EnviromentFile::set(std::string key, optional<std::string> value){
    _data.insert(std::make_pair(key,value.value_or(std::string())));
}

void EnviromentFile::append(map<std::string,std::string> data, bool overwrite){
    for (EnviromentFile::iterator iterator = _data.begin(); iterator != data.end(); iterator++) {
        bool already_exists = _data.find(iterator->first) != _data.end();
        if (already_exists && overwrite ||! already_exists ) {
            _data.insert(std::make_pair(iterator->first, iterator->second));
        }
    }
}

EnviromentFile::iterator EnviromentFile::begin(){
    return _data.begin();
}

EnviromentFile::iterator EnviromentFile::end(){
    return _data.end();
}
