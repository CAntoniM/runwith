
#ifdef TEST_ENVIROMENT_FILE
#include "test_utils.hpp"
#include <filesystem>

#define TEST_FILE "test.txt"
#endif
#include "enviroment_file.hpp"

#include <sstream>
#include <optional>
#include <algorithm> 
#include <cctype>
#include <locale>

using std::map;
using std::ifstream;
using std::ofstream;
using std::string;
using std::optional;

// trim from start (in place)
static inline void ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    }));
}

// trim from end (in place)
static inline void rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
        return !std::isspace(ch);
    }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s) {
    rtrim(s);
    ltrim(s);
}

#ifdef TEST_ENVIROMENT_FILE
TEST_INIT(EnviromentFile);
#endif

#ifdef TEST_ENVIROMENT_FILE
TEST(constructor, a test to ensure the default constructor works) {
    //Writing out a test file to disk so that if a file gets read
    //There will be values in the EnviromentFile object
    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file.close();

    EnviromentFile file;

    TEST_ASSERT(file.get("TEST") == std::nullopt,ensuring that no file has been read.);

    //clean up after ourself
    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}
#endif

void EnviromentFile::load(std::string file_name){
    ifstream file;
    file.exceptions(ifstream::badbit);
    
    file.open(file_name);

    if (!file) throw NoSuchFileException();
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    read(buffer.str());
}

void EnviromentFile::read(std::string text){
    std::stringstream buffer(text);
    std::string line;
    while (std::getline(buffer,line))
    {
        trim(line);

        if (line.empty()) continue;
        if (line.at(0) == '#') continue;
        size_t split_posistion = line.find('=');

        if (split_posistion == string::npos) throw BadFileFormatException();
    

        string key = line.substr(0,split_posistion);
        trim(key);
        string value = line.substr(split_posistion+1);
        trim(value);
        _data.insert(std::make_pair(key,value));

    } 
}

#ifdef TEST_ENVIROMENT_FILE
TEST(unread_get, a test ensuring that pre read and pre set that the enviroment file is clean) 
{
    EnviromentFile file;
    
    TEST_ASSERT(file.get("TEST") == std::nullopt, when no file has been read that nullopt is returned);
}

TEST(file_not_exist,testing that if a file does not exist on disk that we thrown an exception)
{
    
    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);
        TEST_ASSERT(false, testing an exception is thrown when a file is not on disk);
    }catch(...) {
        TEST_ASSERT(true, testing that when a file does exsist in the correct format is read correctly);
    }

}

TEST(normal_file_read, testing if a file is in the correct format it is read){

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file.close();

    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);
        TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")}, testing that the value in memory correctly reflects what is on disk);
    }catch(...) {
        TEST_ASSERT(false, testing that if a file of a correct format exists no exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}

TEST(invalid_file_read, testing if a file is not in the correct format it is read){

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl
              << "TEST TEST" << std::endl;
    test_file.close();

    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);

        TEST_ASSERT(false,testing that no values are inserted into the enviroment file if the file is not of the correct format);

    }catch(...) {
        TEST_ASSERT(true, testing that if a file of a incorrect format exists an exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}

TEST(read_value_trimming, a test to ensure that values in a env file are trimmed of any padded white space)
{

    std::ofstream test_file(TEST_FILE);
    test_file << " TEST = TEST " << std::endl;
    test_file.close();

    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);

        TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that the values have been correctly arsed into memory);

    }catch(...) {
        TEST_ASSERT(false, testing that if a file of a incorrect format exists an exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}

TEST(read_comment_removal, testing that comments are removed from the final output)
{

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file << "#TEST1=TEST" << std::endl;
    test_file << "  #TEST2=TEST" << std::endl;
    test_file.close();

    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);

        TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);
        TEST_ASSERT(file.get("TEST1") == std::nullopt,testing comments are removed when in the most basic structure);
        TEST_ASSERT(file.get("TEST2") == std::nullopt,testing comments are removed when in a more complex structure);

    }catch(...) {
        TEST_ASSERT(false, testing that if a file of a incorrect format exists an exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}

TEST(read_blank_line_removal, testing that comments are removed from the final output)
{

    std::ofstream test_file(TEST_FILE);
    test_file << "TEST=TEST" << std::endl;
    test_file << "" << std::endl;
    test_file << "TEST1=TEST" << std::endl;
    test_file.close();

    EnviromentFile file;
    
    try {
        file.load(TEST_FILE);

        TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);
        TEST_ASSERT(file.get("TEST1") ==  std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);
    }catch(...) {
        TEST_ASSERT(false, testing that if a file of a incorrect format exists an exception is thrown);
    }

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
}
#endif

void EnviromentFile::save(std::string file_name){
        
    std::string text =  this->to_string();
    if (text.empty()) throw BadFileFormatException();

    ofstream file;
    file.exceptions(ofstream::badbit);
    file.open(file_name);

    file << text << std::endl;

    file.close();
}

std::string EnviromentFile::to_string() {
    std::stringstream buffer;

    for( EnviromentFile::iterator iterator = begin(); iterator != end(); iterator++) {
            buffer << iterator->first << " = " << iterator->second << std::endl;
    }

    return buffer.str();
}

#ifdef TEST_ENVIROMENT_FILE
TEST(write, a test for the write function) {
    EnviromentFile write_file;

    write_file.set("TEST",std::optional<std::string>{"TEST"});
    write_file.set("TEST1",std::optional<std::string>{"TEST"});
    write_file.set("TEST2",std::optional<std::string>{"TEST"});

    write_file.save(TEST_FILE);

    EnviromentFile read_file;
    read_file.load(TEST_FILE);

    TEST_ASSERT(read_file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);
    TEST_ASSERT(read_file.get("TEST1") ==  std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);
    TEST_ASSERT(read_file.get("TEST2") ==  std::optional<std::string>{std::string("TEST")},testing that valid inputs are in the file properly);

    TEST_ASSERT(std::filesystem::remove(TEST_FILE),Failed to remove test file);
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
TEST(set, a test to prove the basic of set functonality ) {
    EnviromentFile file;

    file.set("TEST",std::optional<string>{"TEST"});
    TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
}

TEST(set_padding, proving that padded keys will be trimmed down to valid keys) {
    EnviromentFile file;
    
    file.set("\t  TEST   \t",std::optional<string>{"TEST"});
    TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
}

TEST(set_blank, proving that padded keys will be trimmed down to valid keys) {
    EnviromentFile file;
    try {
        file.set("",std::optional<string>{"TEST"});
        TEST_ASSERT(false,testing that an exception is thrown when no key is provided);
    } catch (...) {
        TEST_ASSERT(true,testing that an exception is thrown when no key is provided);
    }

}
#endif

void EnviromentFile::set(std::string key, optional<std::string> value){
    trim(key);

    if (key.empty()) throw BadFileFormatException();
    _data.insert(std::make_pair(key,value.value_or(std::string())));
}

void EnviromentFile::append(map<std::string,std::string> data, bool overwrite){

    if (data.find("") != data.end()) throw BadFileFormatException();

    for (EnviromentFile::iterator iterator = data.begin(); iterator != data.end(); iterator++) {
        bool already_exists = _data.find(iterator->first) != _data.end();
        if (already_exists && overwrite ||! already_exists ) {
            this->set(iterator->first,std::optional<std::string>{iterator->second});
        }
    }
}

#ifdef TEST_ENVIROMENT_FILE
TEST(append, test for basic append functonality) {
    EnviromentFile file;

    std::map<std::string,std::string> data;

    data.insert(std::make_pair(std::string("TEST"),std::string("TEST")));
    data.insert(std::make_pair(std::string("TEST1"),std::string("TEST")));
    data.insert(std::make_pair(std::string("TEST2"),std::string("TEST")));
    data.insert(std::make_pair(std::string("TEST3"),std::string("TEST")));
    
    file.append(data);
    
    TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST1") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST2") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST3") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
}

TEST(append_padding, testing to ensure that whitespace is trimmed from keys set via append) {
    EnviromentFile file;

    std::map<std::string,std::string> data;

    data.insert(std::make_pair(std::string("TEST"),std::string("TEST")));
    data.insert(std::make_pair(std::string(" TEST1 "),std::string("TEST")));
    data.insert(std::make_pair(std::string("\tTEST2\t"),std::string("TEST")));
    data.insert(std::make_pair(std::string("\nTEST3\n"),std::string("TEST")));
    
    file.append(data);
    
    TEST_ASSERT(file.get("TEST") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST1") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST2") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
    TEST_ASSERT(file.get("TEST3") == std::optional<std::string>{std::string("TEST")},testing that set values can be reterived);
}

TEST(append_blank, testing that an error is thrown if any of the keys are blank and that no value is set in memory) {
    EnviromentFile file;

    std::map<std::string,std::string> data;

    data.insert(std::make_pair(std::string("TEST"),std::string("TEST")));
    data.insert(std::make_pair(std::string(""),std::string("TEST")));
    data.insert(std::make_pair(std::string("TEST2"),std::string("TEST")));
    data.insert(std::make_pair(std::string("TEST3"),std::string("TEST")));

    try {
        file.append(data);
        TEST_ASSERT(false,testing that an exception is thrown when no key is provided);
    } catch (...) {
        TEST_ASSERT(true,testing that an exception is thrown when no key is provided);
    }
    TEST_ASSERT(file.get("TEST") == std::nullopt,testing that no values are set when an empty key is appended);

}
#endif

EnviromentFile::iterator EnviromentFile::begin(){
    return _data.begin();
}

EnviromentFile::iterator EnviromentFile::end(){
    return _data.end();
}
