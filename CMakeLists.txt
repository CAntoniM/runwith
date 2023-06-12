cmake_minimum_required(VERSION 3.10)

set(Boost_USE_MULTITHREADED ON)
find_package(Boost 1.64.0 REQUIRED)
if(Boost_FOUND)
  include_directories (SYSTEM ${Boost_INCLUDE_DIR})
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(runwith)
file(GLOB SOURCES "src/*.cpp")
add_executable(run_with ${SOURCES})
target_compile_features(run_with PRIVATE cxx_std_17)
target_include_directories(run_with PUBLIC inc)
target_link_libraries (run_with ${Boost_LIBRARIES})

enable_testing()

# Testing in this project is handled in memeory by the test_utils header 
# and requires that an application be built with TEST_<FILE_NAME> be defined
# it then just requires that you compile and link the source file in question 
# into an exe with all its dependancies 

add_executable(test_enviroment_file src/enviroment_file.cpp)
target_include_directories(test_enviroment_file PUBLIC inc)
target_compile_definitions(test_enviroment_file PUBLIC TEST_ENVIROMENT_FILE)

add_test(NAME EnviromentFile COMMAND test_enviroment_file -C Release)

add_executable(test_cli src/cli.cpp)
target_include_directories(test_cli PUBLIC inc)
target_compile_definitions(test_cli PUBLIC TEST_CLI)

add_test(NAME Cli COMMAND test_cli -C Release)
