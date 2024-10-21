cmake_minimum_required(VERSION 3.1)
project(huawei VERSION 1.0 DESCRIPTION "Hackathon")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


include_directories(include)


# Declare libraries
add_library(scheduler src/Scheduler.cpp)
add_library(packet src/Packet.cpp)
add_library(slice src/Slice.cpp)

add_executable(target main.cpp)

# Link libraries
target_link_libraries(target PUBLIC slice packet scheduler)



# Install the executable to bin directory
install(TARGETS target DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
