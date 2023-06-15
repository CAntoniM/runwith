# Run with

A simple wrapper for running commands with a .env file applied on top of them.

Why write this.

well I hate enviroment variables, they are messy unchecked and uncontrollable,
why did this test fail ohh some one changed an enviroment variable, or why 
don't my commands variables work any more ohh i fucked my enviroment variables
etc, 

how do we stop the random changing of values we care about. simple write them
down in a file and have a programme apply them before running commands. 
as they are set by a child process they will not bleed upto us. and as they
in a file you know what they are set too.

## Examples

to use the programme is quite simple sipmle prefix the command with runwith

```
runwith ls -al
```

this will automaically read the .env file and apply it or you can set which file
you want to read using the --env flag

```
runwith --env .my_env ls -al
```

you can also see what env is being set by enabling verbose output seen by the
following.

```
runwith --verbose --env .my_env ls -al
```

## Project structure 

As most cpp projects do we require 2 dependancies:

1. [cmake](https://cmake.org/) -> a cross platform build tool (and the only reason I still have hair)
2. [boost](https://www.boost.org/) -> a library that fixes all the missing features of the STL

you can get these dependancies anyway you like (most OS's have a package manager now)
however for boost the prefered method is to use [conan](https://conan.io/) as it will ensure that
anyone who wants to build on the project is using the same library version as a base

### Building 

first install all of you dependancies below is the method on windows for linux consult your distro
and we are assuming you have a cpp compiler install via visual studio or your ditros package manager

```
winget install cmake python3;
pip install conan

conan profile detect --force

conan install . --output-folder=build --build=missing
```

this will install cmake and python3 it will then use pythons package manager to install conan
it will then use conan to install boost. next we follow the standard cmake practice;

```
cd build;
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan.toolchain.cmake
cmake --build . --config Release
```

this should then build 

### Testing 

Testing for this project is structured into the 2 layers the first is the module level
runner who's job to execute each of the module tests and display a passed or failed
and print the output if they fail. in our case we use ctest due to its intergration
into cmake. to run ctest you should do this

```
cd build;
ctest -C Release --output-on-failure .
```

for more details see the ctest documentation

This will run all of the tests that the product has.

the second layer is the unit level runner its job is to go over each test case and assert and ensure they are fine 
and docuemnt them. this is done via a homespun solution called test_utils.hpp. to run these tests you simply need
to execute the executable.

```
./build/test_cli.exe
```

however you can also enable verbose logging which will print every test and assert regardless of status

```
./build/test_cli.exe [--verbose|-v]
```

another option is to put the output into a json file for pasing by another programme.

```
./build/test_cli.exe [--output|-o] <output_file>
```

finally you can also choose to run only a subset of tests this can be useful if you are debugging a 
specific issue and only want to see the results from one test.

```
./build/test_cli.exe [OPTIONS] [TEST_NAME] ...
``` 