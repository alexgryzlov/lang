# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.17

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Disable VCS-based implicit rules.
% : %,v


# Disable VCS-based implicit rules.
% : RCS/%


# Disable VCS-based implicit rules.
% : RCS/%,v


# Disable VCS-based implicit rules.
% : SCCS/s.%


# Disable VCS-based implicit rules.
% : s.%


.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

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
CMAKE_COMMAND = /opt/clion-2020.1.3/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /opt/clion-2020.1.3/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/alex/gitrepos/lang

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/alex/gitrepos/lang

# Include any dependencies generated for this target.
include Compiler/CMakeFiles/xzyc.dir/depend.make

# Include the progress variables for this target.
include Compiler/CMakeFiles/xzyc.dir/progress.make

# Include the compile flags for this target's objects.
include Compiler/CMakeFiles/xzyc.dir/flags.make

Compiler/CMakeFiles/xzyc.dir/main.cpp.o: Compiler/CMakeFiles/xzyc.dir/flags.make
Compiler/CMakeFiles/xzyc.dir/main.cpp.o: Compiler/main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/alex/gitrepos/lang/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object Compiler/CMakeFiles/xzyc.dir/main.cpp.o"
	cd /home/alex/gitrepos/lang/Compiler && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/xzyc.dir/main.cpp.o -c /home/alex/gitrepos/lang/Compiler/main.cpp

Compiler/CMakeFiles/xzyc.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/xzyc.dir/main.cpp.i"
	cd /home/alex/gitrepos/lang/Compiler && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/alex/gitrepos/lang/Compiler/main.cpp > CMakeFiles/xzyc.dir/main.cpp.i

Compiler/CMakeFiles/xzyc.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/xzyc.dir/main.cpp.s"
	cd /home/alex/gitrepos/lang/Compiler && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/alex/gitrepos/lang/Compiler/main.cpp -o CMakeFiles/xzyc.dir/main.cpp.s

# Object files for target xzyc
xzyc_OBJECTS = \
"CMakeFiles/xzyc.dir/main.cpp.o"

# External object files for target xzyc
xzyc_EXTERNAL_OBJECTS =

Compiler/xzyc: Compiler/CMakeFiles/xzyc.dir/main.cpp.o
Compiler/xzyc: Compiler/CMakeFiles/xzyc.dir/build.make
Compiler/xzyc: Compiler/CMakeFiles/xzyc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/alex/gitrepos/lang/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable xzyc"
	cd /home/alex/gitrepos/lang/Compiler && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/xzyc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Compiler/CMakeFiles/xzyc.dir/build: Compiler/xzyc

.PHONY : Compiler/CMakeFiles/xzyc.dir/build

Compiler/CMakeFiles/xzyc.dir/clean:
	cd /home/alex/gitrepos/lang/Compiler && $(CMAKE_COMMAND) -P CMakeFiles/xzyc.dir/cmake_clean.cmake
.PHONY : Compiler/CMakeFiles/xzyc.dir/clean

Compiler/CMakeFiles/xzyc.dir/depend:
	cd /home/alex/gitrepos/lang && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/alex/gitrepos/lang /home/alex/gitrepos/lang/Compiler /home/alex/gitrepos/lang /home/alex/gitrepos/lang/Compiler /home/alex/gitrepos/lang/Compiler/CMakeFiles/xzyc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Compiler/CMakeFiles/xzyc.dir/depend

