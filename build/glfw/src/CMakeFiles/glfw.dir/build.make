# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


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
CMAKE_COMMAND = /Applications/CMake.app/Contents/bin/cmake

# The command to remove a file.
RM = /Applications/CMake.app/Contents/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/ziqwang/Documents/Xcode/TI

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/ziqwang/Documents/Xcode/TI/build

# Include any dependencies generated for this target.
include glfw/src/CMakeFiles/glfw.dir/depend.make

# Include the progress variables for this target.
include glfw/src/CMakeFiles/glfw.dir/progress.make

# Include the compile flags for this target's objects.
include glfw/src/CMakeFiles/glfw.dir/flags.make

# Object files for target glfw
glfw_OBJECTS =

# External object files for target glfw
glfw_EXTERNAL_OBJECTS = \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/context.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/init.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/input.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/monitor.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/vulkan.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/window.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_init.m.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_joystick.m.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_monitor.m.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_window.m.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/cocoa_time.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/posix_tls.c.o" \
"/Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw_objects.dir/nsgl_context.m.o"

glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/context.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/init.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/input.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/monitor.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/vulkan.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/window.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/cocoa_init.m.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/cocoa_joystick.m.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/cocoa_monitor.m.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/cocoa_window.m.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/cocoa_time.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/posix_tls.c.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw_objects.dir/nsgl_context.m.o
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw.dir/build.make
glfw/src/libglfw.3.2.dylib: glfw/src/CMakeFiles/glfw.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/ziqwang/Documents/Xcode/TI/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Linking C shared library libglfw.dylib"
	cd /Users/ziqwang/Documents/Xcode/TI/build/glfw/src && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/glfw.dir/link.txt --verbose=$(VERBOSE)
	cd /Users/ziqwang/Documents/Xcode/TI/build/glfw/src && $(CMAKE_COMMAND) -E cmake_symlink_library libglfw.3.2.dylib libglfw.3.dylib libglfw.dylib

glfw/src/libglfw.3.dylib: glfw/src/libglfw.3.2.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate glfw/src/libglfw.3.dylib

glfw/src/libglfw.dylib: glfw/src/libglfw.3.2.dylib
	@$(CMAKE_COMMAND) -E touch_nocreate glfw/src/libglfw.dylib

# Rule to build all files generated by this target.
glfw/src/CMakeFiles/glfw.dir/build: glfw/src/libglfw.dylib

.PHONY : glfw/src/CMakeFiles/glfw.dir/build

glfw/src/CMakeFiles/glfw.dir/requires:

.PHONY : glfw/src/CMakeFiles/glfw.dir/requires

glfw/src/CMakeFiles/glfw.dir/clean:
	cd /Users/ziqwang/Documents/Xcode/TI/build/glfw/src && $(CMAKE_COMMAND) -P CMakeFiles/glfw.dir/cmake_clean.cmake
.PHONY : glfw/src/CMakeFiles/glfw.dir/clean

glfw/src/CMakeFiles/glfw.dir/depend:
	cd /Users/ziqwang/Documents/Xcode/TI/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/ziqwang/Documents/Xcode/TI /Users/ziqwang/Documents/GitHub/libigl/external/nanogui/ext/glfw/src /Users/ziqwang/Documents/Xcode/TI/build /Users/ziqwang/Documents/Xcode/TI/build/glfw/src /Users/ziqwang/Documents/Xcode/TI/build/glfw/src/CMakeFiles/glfw.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : glfw/src/CMakeFiles/glfw.dir/depend
