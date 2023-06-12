#ifndef _ENVIROMENT_FILE_H_
#define _ENVIROMENT_FILE_H_

#include <map>
#include <iostream>
#include <fstream>
#include <string>
#include <optional>

using std::map;
using std::ifstream;
using std::string;
using std::optional;

class NoSuchFileException : public std::exception {
    public:
char * what () {
        return "No such file or directory.";
    }
};

class BadFileFormatException : public std::exception {
    public:
char * what () {
        return "File is of an invalid file format.";
    }
};


/**
 * @brief a class representing a .env file and allowing its manipulation
 * 
 */
class EnviromentFile {
    public:
        EnviromentFile(string file_name);
        /**
         * @brief changes the file name in memeory used to change where we write
         * the file to.
         * 
         * @param file_name 
         */
        void file_name(string file_name);
        /**
         * @brief This returns the file name we are reading and writing from.
         * 
         * @return string 
         */
        string file_name();
        /**
         * @brief reads the file from disk into memeory 
         * 
         */
        void read();
        /**
         * @brief write from memory to disk
         * 
         */
        void write();
        /**
         * @brief This gets the value at that key if it exists.
         * 
         * @param key 
         * @return optional<string> 
         */
        optional<string> get(string key);
        /**
         * @brief This will set the value of the item at that key, however if
         * none is given it will remove any value at that key
         * 
         * @param key 
         * @param value 
         */
        void set(string key, optional<string> value);
        /**
         * @brief This appends hash map to the enviroment file this will overwirte 
         * values already in the data set overwrite to false if this is not the
         * intended behaviour
         * 
         * @param data 
         */
        void append(map<string,string> data, bool overwrite = true);
        /**
         * @brief this is the type of the iterator for this class
         * 
         */
        typedef map<string,string>::iterator iterator;
        /**
         * @brief This returns an iterator to the start of the map
         * 
         * @return iterator 
         */
        iterator begin();
        /**
         * @brief This returns an iterator to the end of the map
         * 
         * @return iterator 
         */
        iterator end();

    private:
        string _file_name;
        map<string, string> _data;
};

#endif
