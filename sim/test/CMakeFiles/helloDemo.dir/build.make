# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list

# Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = "/Applications/CMake 2.8-12.app/Contents/bin/cmake"

# The command to remove a file.
RM = "/Applications/CMake 2.8-12.app/Contents/bin/cmake" -E remove -f

# Escaping for special characters.
EQUALS = =

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = "/Applications/CMake 2.8-12.app/Contents/bin/ccmake"

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jernej/repo/arbot/sim/test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jernej/repo/arbot/sim/test

# Include any dependencies generated for this target.
include CMakeFiles/helloDemo.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/helloDemo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/helloDemo.dir/flags.make

CMakeFiles/helloDemo.dir/backprop.cpp.o: CMakeFiles/helloDemo.dir/flags.make
CMakeFiles/helloDemo.dir/backprop.cpp.o: backprop.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /Users/jernej/repo/arbot/sim/test/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/helloDemo.dir/backprop.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/helloDemo.dir/backprop.cpp.o -c /Users/jernej/repo/arbot/sim/test/backprop.cpp

CMakeFiles/helloDemo.dir/backprop.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/helloDemo.dir/backprop.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /Users/jernej/repo/arbot/sim/test/backprop.cpp > CMakeFiles/helloDemo.dir/backprop.cpp.i

CMakeFiles/helloDemo.dir/backprop.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/helloDemo.dir/backprop.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /Users/jernej/repo/arbot/sim/test/backprop.cpp -o CMakeFiles/helloDemo.dir/backprop.cpp.s

CMakeFiles/helloDemo.dir/backprop.cpp.o.requires:
.PHONY : CMakeFiles/helloDemo.dir/backprop.cpp.o.requires

CMakeFiles/helloDemo.dir/backprop.cpp.o.provides: CMakeFiles/helloDemo.dir/backprop.cpp.o.requires
	$(MAKE) -f CMakeFiles/helloDemo.dir/build.make CMakeFiles/helloDemo.dir/backprop.cpp.o.provides.build
.PHONY : CMakeFiles/helloDemo.dir/backprop.cpp.o.provides

CMakeFiles/helloDemo.dir/backprop.cpp.o.provides.build: CMakeFiles/helloDemo.dir/backprop.cpp.o

# Object files for target helloDemo
helloDemo_OBJECTS = \
"CMakeFiles/helloDemo.dir/backprop.cpp.o"

# External object files for target helloDemo
helloDemo_EXTERNAL_OBJECTS =

helloDemo: CMakeFiles/helloDemo.dir/backprop.cpp.o
helloDemo: CMakeFiles/helloDemo.dir/build.make
helloDemo: CMakeFiles/helloDemo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX executable helloDemo"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/helloDemo.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/helloDemo.dir/build: helloDemo
.PHONY : CMakeFiles/helloDemo.dir/build

CMakeFiles/helloDemo.dir/requires: CMakeFiles/helloDemo.dir/backprop.cpp.o.requires
.PHONY : CMakeFiles/helloDemo.dir/requires

CMakeFiles/helloDemo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/helloDemo.dir/cmake_clean.cmake
.PHONY : CMakeFiles/helloDemo.dir/clean

CMakeFiles/helloDemo.dir/depend:
	cd /Users/jernej/repo/arbot/sim/test && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jernej/repo/arbot/sim/test /Users/jernej/repo/arbot/sim/test /Users/jernej/repo/arbot/sim/test /Users/jernej/repo/arbot/sim/test /Users/jernej/repo/arbot/sim/test/CMakeFiles/helloDemo.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/helloDemo.dir/depend
