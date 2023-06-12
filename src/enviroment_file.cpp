
#ifdef TEST_ENVIROMENT_FILE
#include "test_utils.hpp"
#include <filesystem>

#define TEST_FILE "test.txt"
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
TEST_INIT(EnviromentFile);
#endif

EnviromentFile::EnviromentFile(string file_name){
    this->file_name(file_name);
}

#ifdef TEST_ENVIROMENT_FILE
TEST(constructor, a test to ensure the default constructor works) {
    //Writing out a test file to disk so that if a file gets read
    //There will be values in the EnviromentFile object
    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file.close();

    EnviromentFile file(TEST_FILE);

    TEST_ASSERT(file.file_name() == TEST_FILE,ensuring that the file name is set properly during the constructor);
    TEST_ASSERT(file.get("TEST") == std::nullopt,ensuring that no file has been read.);

    //clean up after ourself
    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
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
    
    file.open(_file_name);

    if (!file) throw NoSuchFileException();
    
    string line;
    
    while (std::getline(file,line))
    {
        size_t split_posistion = line.find('=');

        if (split_posistion == string::npos) {
            throw BadFileFormatException();
        }

        string key = line.substr(0,split_posistion);
        string value = line.substr(split_posistion+1);
        _data.insert(std::make_pair(key,value));

    } 
}

#ifdef TEST_ENVIROMENT_FILE
TEST(unread_get, a test ensuring that pre read and pre set that the enviroment file is clean) 
{
    EnviromentFile file(TEST_FILE);
    
    TEST_ASSERT(file.get("TEST") == std::nullopt, when no file has been read that nullopt is returned);
}

TEST(file_not_exist,testing that if a file does not exist on disk that we thrown an exception)
{
    
    EnviromentFile file(TEST_FILE);
    
    try {
        file.read();
        TEST_ASSERT(false, testing an exception is thrown when a file is not on disk);
    }catch(...) {
        TEST_ASSERT(true, testing that when a file does exsist in the correct format is read correctly);
    }

}

TEST(normal_file_read, testing if a file is in the correct format it is read){

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file.close();

    EnviromentFile file(TEST_FILE);
    
    try {
        file.read();
        TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")}, testing that the value in memory correctly reflects what is on disk);
    }catch(...) {
        TEST_ASSERT(false, testing that if a file of a correct format exists no exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}

TEST(invalid_file_read, testing if a file is not in the correct format it is read){

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST TEST" << std::endl;
    test_file.close();

    EnviromentFile file(TEST_FILE);
    
    try {
        file.read();
        TEST_ASSERT(false, testing that if a file of a incorrect format exists an exception is thrown);
    }catch(...) {
        TEST_ASSERT(true, testing that if a file of a incorrect format exists an exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}
#endif

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

#ifdef TEST_ENVIROMENT_FILE
TEST(write, a test for the write function) {
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
}
#endif

optional<std::string> EnviromentFile::get(std::string key){

    EnviromentFile::iterator position = _data.find(key);

    if (position == _data.end()) {
        return std::nullopt;
    }

    return optional<string>{position->second};
}

#ifdef TEST_ENVIROMENT_FILE
TEST(get_set, a test to prove the basic of the get and set functonality ) {
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
}
#endif

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

#ifdef TEST_ENVIROMENT_FILE
TEST(append, append functonality) {
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
    TEST_ASSERT(true,Figuring out if this works at all);
}
#endif

EnviromentFile::iterator EnviromentFile::begin(){
    return _data.begin();
}

EnviromentFile::iterator EnviromentFile::end(){
    return _data.end();
}
