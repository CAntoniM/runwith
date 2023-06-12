
#ifndef _TEST_UTILS_H_
#define _TEST_UTILS_H_

#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <functional>
#include <iostream>
#include <memory>


/**
 * @brief This is a series of test helpers allowing for better unit tests
 * and logging its results properly to allow for writing tests with minimal 
 * overhead
 * 
 * its general goals are:
 * 
 * 1. minimise external depdancies: if its not in the std c or c++ libs do we
 *    need it
 * 2. minimise the amount of effort required by the end user.
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
 * @brief a representation of thecurrent test status.
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
bool register_test(std::string name, std::string objective,std::string file,size_t line, std::function<void()> test_func) {

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
bool test_assert(bool result, std::string objective, std::string file, size_t line) {
    std::shared_ptr<Test> test = get_current_test();
    Status assert_status;
    if (!result) {
        test->status = Status::Failed;
        assert_status = Status::Failed;
        test->outputbuffer << "\t\t Assert Failed"<< "@" << file << ":" << line << "\t" << objective << std::endl;
    } else {
        assert_status = Status::Passed;
        if (verbose) {
            test->outputbuffer << "\t\t Assert Passed"<< "@" << file << ":" << line << "\t" << objective << std::endl;
        }
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
 * @brief This is our main implementaion for the TEST_INIT macro
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int run_all_tests (int argc, char* argv[]) {
    for (int index = 0; index < argc; index++) {
        if (strcmp(argv[index],"-v") || strcmp(argv[index],"--verbose")) {
            verbose = true;
        }
    }
    Status status = Status::Running;
    for(std::map<std::string,std::shared_ptr<Test>>::iterator iterator = results.begin(); iterator != results.end(); iterator ++) {
        std::shared_ptr<Test> test = iterator->second;
        std::cout << "Running test " << test->name << "@" << test->file << ":" << test->line << "\t";
        test->status = Status::Running;
        test->test_func();
        if (test->status != Failed) {
            test->status = Passed;
            std::cout << "Passed" << std::endl;
            if (verbose) {
                std::cout << test->outputbuffer.str();
            }
        }else {
            status = Status::Failed;
            std::cout << "Failed" << std::endl << test->outputbuffer.str();
        }
        std::cout << std::endl;
    }

    if (status == Status::Failed) {
        return 1;
    }
    return 0;
}

#define CONCAT(FIRST,SECOND) FIRST##SECOND
#define STRINGIFY(TXT) #TXT

#define TEST(NAME,OBJECTIVE) void CONCAT(__test_,NAME) (); bool CONCAT(__test_var_,NAME) = register_test( STRINGIFY(NAME), STRINGIFY(OBJECTIVE), __FILE__ , __LINE__ , CONCAT(__test_,NAME) ); void CONCAT(__test_,NAME) ()

#define TEST_INIT() int main (int argc, char* argv[] ) { return run_all_tests(argc,argv);}

#define TEST_ASSERT(TEST,OBJECTIVE) test_assert(TEST,STRINGIFY(OBJECTIVE),__FILE__,__LINE__)

#endif
