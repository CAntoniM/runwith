
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>   
#include <chrono>


/**
 * @brief This is a series of test helpers allowing for better unit tests
 * and logging its results properly to allow for writing tests with minimal 
 * overhead
 * 
 * its general goals are:
 * 
 * 1. minimise external depdancies: if its not in the std c or c++ libs we should avoid it.
 * 2. minimise the amount of effort required write tests.
 * 3. minimise the distance between the test code and the source code.
 * 
 * to achive this the idea is that we have this header file which contains all
 * the logic, test_runner.cpp which contains the main function and driver code
 * and the source file of the programme you are wanting to test.
 * 
 * the source file contains code that will look like the following
 * 
 * void foo() {...}
 * 
 * #ifdef TESTING
 * TEST(foo, this is a test of food functonaility) {...}
 * #endif
 * 
 */

/**
 * @brief a representation of thecurrent test status
 * 
 */
enum Status {
    Unrun,
    Running,
    Passed,
    Failed,
};

/**
 * @brief an object representing an assert that has happened during the test.
 * 
 */
class Assert {
    public:
    std::string name;
    std::string objective;
    std::string file;
    size_t line;
    Status status;
};

/**
 * @brief an representation of a test all of its consituate components
 * 
 */
class Test {
    public:
    std::string name;
    std::string objective;
    Status status;
    std::string file;
    size_t line;
    std::chrono::microseconds test_duration;
    std::vector<std::shared_ptr<Assert>> asserts;
    std::function<void()> test_func;
    std::ostringstream outputbuffer;
};

/**
 * @brief a map containing all of the current tests that are to be run as well
 * as there results;
 * 
 */
std::map<std::string,std::shared_ptr<Test>> results;
/**
 * @brief a store for the commandline flag to enable more logging from the test
 * framework
 * 
 */
bool verbose = false;

/**
 * @brief Creates a test and registers it with the test list for running by the
 * TEST_INIT function
 * 
 * @param name 
 * @param objective 
 * @param file 
 * @param line 
 * @param test_func 
 * @return true 
 * @return false 
 */
bool register_test(std::string name, std::string objective,std::string path,size_t line, std::function<void()> test_func) {

#ifdef _WIN32
    std::string file = path.substr(path.find_last_of("\\") + 1); 
#else
    std::string file = path.substr(path.find_last_of("/") + 1); 
#endif

    if (results.find(name) != results.end()) {
        std::cerr << "ERROR: " << "failed to register test " << name << " because the name is already taken" << std::endl;
        exit(-1);
    }

    std::shared_ptr<Test> test (new Test());

    test->name = name;
    test->objective = objective;
    test->status = Status::Unrun;
    test->file = file;
    test->line = line;
    test->test_func = test_func;

    results[name] = test;

    return true;
}

/**
 * @brief Get the current test object returns whatever the current test being ran is
 * 
 * @return std::shared_ptr<Test> 
 */
std::shared_ptr<Test> get_current_test() {
    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
        if (iterator->second->status == Status::Running) {
            return iterator->second;
        }
    }

    std::cerr << "ERROR: failed to get a currently running test" << std::endl;
    exit(-1);
}

/**
 * @brief This creates an assert and then registers it with the currently running test
 * 
 * @param result 
 * @param objective 
 * @param file 
 * @param line 
 * @return true 
 * @return false 
 */
bool test_assert(bool result, std::string objective, std::string path, size_t line) {

#ifdef _WIN32
    std::string file = path.substr(path.find_last_of("\\") + 1); 
#else
    std::string file = path.substr(path.find_last_of("/") + 1); 
#endif

    std::shared_ptr<Test> test = get_current_test();
    Status assert_status;
    
    if (verbose || !result) {
        test->outputbuffer << "    Assert"<< "@" << file << ":" << line << " -> ";
    }

    if (!result) {
        test->status = Status::Failed;
        assert_status = Status::Failed;
        test->outputbuffer << "FAILED";
    } else {
        assert_status = Status::Passed;
        test->outputbuffer << "PASSED";
    }

    if (verbose || !result) {
        test->outputbuffer << "\t" << objective << std::endl;
    }

    std::shared_ptr<Assert> _assert(new Assert());
    
    _assert->file = file;
    _assert->line = line;
    _assert->objective = objective;
    _assert->status = assert_status;
    
    test->asserts.push_back(_assert);
    
    return result;
}

/**
 * This generates the unique idenfier for the test given
*/
std::string generate_test_id(std::string prefix, std::shared_ptr<Test> test) {
    std::ostringstream id_stream;
    id_stream << prefix << "::" << test->name << "@" << test->file << ":" << test->line;
    return id_stream.str();
}

/**
 * Returns the maximum length of a test id
*/
size_t max_test_id_length(std::string prefix) {
    size_t max_length = 0;
    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
        size_t length = generate_test_id(prefix,iterator->second).length();

        if (length > max_length) {
            max_length = length;
        }
    }

    return max_length;
}

/**
 * This returns the number of tests that have been registered
*/
size_t number_of_tests() {
    size_t count = 0;

    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
        count++;
    }

    return count;
}

/**
 * This is a helper function for getting the number of tests that have failed at this point
*/
size_t number_of_failed_tests() {
    size_t count = 0;

    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator++) {
        if (iterator->second->status == Status::Passed) {
            count++;
        }
    }

    return count;
}

/**
 * Helper function for generating out the spacer that is put in after the test id this is done to make the output pretty
*/
std::string generate_spacer(size_t max_length, size_t current_length, size_t extra_space) {
    std::ostringstream spacer_buffer;
    spacer_buffer << " ";
    for (current_length; current_length <= max_length + extra_space; current_length++) {
        spacer_buffer << ".";
    }
    spacer_buffer << " ";
    return spacer_buffer.str();
}

/**
 * @brief This is our main implementaion for the TEST_INIT macro
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int run_all_tests (int argc, char* argv[], std::string prefix) {
    
    Status status = Status::Running;
    size_t max_id_size = max_test_id_length(prefix);

    for (int index = 0; index < argc; index++) {
        if (strcmp(argv[index],"-v") == 0 || strcmp(argv[index],"--verbose") == 0) {
            verbose = true;
        }
    }
    
    std::cout << "\nStarting testing for: " << prefix << std::endl << std::endl;
    auto start_time = std::chrono::high_resolution_clock::now();
    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator ++) {
        
        std::shared_ptr<Test> test = iterator->second;
        std::string test_id = generate_test_id(prefix,test);
        std::string spacer = generate_spacer(max_id_size,test_id.length(),4);
        
        std::cout << "Running test " << test_id << spacer;
        
        test->status = Status::Running;
        auto test_start_time = std::chrono::high_resolution_clock::now();
        test->test_func();
        test->test_duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - test_start_time);
        if (test->status != Failed) {

            test->status = Passed;
       
            std::cout << "Passed (in " << test->test_duration.count() << "us)" << std::endl;
       
            if (verbose) {
                std::cout << "\n    Objective: " << test->objective << '\n' << std::endl << test->outputbuffer.str() << std::endl;
            }
       
        }else {

            status = Status::Failed;

            std::cout << "Failed (in " << test->test_duration.count() << "us)" << std::endl; 
            std::cerr << "\n    Objective: " << test->objective << '\n' << std::endl << test->outputbuffer.str() << std::endl;

        }
    std::cout.flush();
}
    std::chrono::microseconds duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - start_time);
    size_t ran_tests = number_of_tests();
    size_t passed_tests = number_of_failed_tests();
    std::cout << std::endl << "Finished running tests " << passed_tests << "/" <<  ran_tests << " tests Passed  (in: "<< duration.count() << "us) ...... Test run ";

    if (status == Status::Failed) {
        std::cout << "Failed!\n" << std::endl;
        return 1;
    }
    std::cout << "Passed!\n" << std::endl;
    return 0;
}

#define CONCAT(FIRST,SECOND) FIRST##SECOND
#define STRINGIFY(TXT) #TXT

#define TEST(NAME,OBJECTIVE) void CONCAT(__test_,NAME) (); bool CONCAT(__test_var_,NAME) = register_test( STRINGIFY(NAME), STRINGIFY(OBJECTIVE), __FILE__ , __LINE__ , CONCAT(__test_,NAME) ); void CONCAT(__test_,NAME) ()

#define TEST_INIT(PREFIX) int main (int argc, char* argv[] ) { return run_all_tests(argc,argv,STRINGIFY(PREFIX));}

#define TEST_ASSERT(TEST,OBJECTIVE) if (!test_assert(TEST,STRINGIFY(OBJECTIVE),__FILE__,__LINE__)) return;

#endif
