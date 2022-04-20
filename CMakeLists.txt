cmake_minimum_required(VERSION 2.8.9)
project(directory_test)
 
set (CMAKE_CXX_STANDARD 17)
#add_compile_options(-fsanitize=address)
#add_link_options(-fsanitize=address)
#Bring the headers, such as Student.h into the project
include_directories(include)
 
#Can manually add the sources using the set command as follows:
#set(SOURCES src/mainapp.cpp src/Student.cpp)
 
#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
set(CMAKE_BUILD_TYPE Debug)
 
add_executable(main ${SOURCES})
