# CMakeLists files in this project can
# refer to the root source directory of the project as ${HELLO_SOURCE_DIR} and
# to the root binary directory of the project as ${HELLO_BINARY_DIR}.
cmake_minimum_required (VERSION 2.8.11)
project (HELLO)
set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

# Recurse into the "Hello" and "Demo" subdirectories. This does not actually
# cause another cmake executable to run. The same process will walk through
# the project's entire directory structure.
include_directories(..)
include_directories(/Volumes/DataHD/MacOs/jernej/Developer/FANN-2.2.0-Source/src/include)

# Create a library called "Hello" which includes the source file "hello.cxx".
# The extension is already found. Any number of sources could be listed here.

# Make sure the compiler can find include files for our Hello library
# when other libraries or executables link to Hello
# Add executable called "helloDemo" that is built from the source files
# "demo.cxx" and "demo_b.cxx". The extensions are automatically found.
add_executable (helloDemo backprop.cpp)

# Link the executable to the Hello library. Since the Hello library has
# public include directories we will use those link directories when building
# helloDemo
#SET(CMAKE_INSTALL_RPATH "/Volumes/DataHD/MacOs/jernej/Developer/FANN-2.2.0-Source/bin") 
#target_link_libraries (helloDemo LINK_PUBLIC floatfann)
target_link_libraries (helloDemo)