cmake_minimum_required(VERSION 3.0.0)
project(Turtle)

##########################################################################################################################
#################################################### Finding Packages ####################################################
##########################################################################################################################

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

#find_package(nlohmann_json REQUIRED)

##########################################################################################################################
###################################################### Actual Stuff ######################################################
##########################################################################################################################

# Include the Necessary Headers
include_directories(
    inc/
    )

# Initialize the Source Variables
file(GLOB SOURCES "src/main.cpp" 
                  "src/*.cpp"
                  "src/*/*.cpp" 
                  "src/*/*/*.cpp" 
                  "src/*/*/*/*.cpp" 
                  "src/*/*/*/*/*.cpp" 
                  "src/*/*/*/*/*/*.cpp")

#file(GLOB SOURCES "src/*.cpp")

# Some global configurations
set(CMAKE_CXX_FLAGS "-Ofast")
set(CMAKE_C_FLAGS "-Ofast")
set (CMAKE_CXX_STANDARD 20)

# Build our Library
# Build our Abstract Server and Flight Controller Interface
add_executable(${PROJECT_NAME} ${SOURCES})
#target_link_libraries(${PROJECT_NAME} Threads::Threads)