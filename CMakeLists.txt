cmake_minimum_required(VERSION 3.0)
# Have CMake find our pthreads library within our toolchain (required for this library)
project (mmdvm CXX)
#set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
#find_package(Threads REQUIRED)
# add all the *.c files as sources
FILE(GLOB SRC_FILES *.cpp)
# make this output a shared library (with .so output)
add_executable(mmdvm ${SRC_FILES})
add_definitions("-g -DRPI") # -mcpu=cortex-a53 -mfloat-abi=hard -mfpu=neon-fp-armv8 -mneon-for-64bits -mtune=cortex-a53" )
# be sure to include the current source directory for header files
target_include_directories (mmdvm PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# add the following required libraries:
target_link_libraries(mmdvm stdc++ m rt pthread zmq)

