cmake_minimum_required (VERSION 3.8)

project("PrimeService")

#CXX
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#Sources
file(GLOB_RECURSE HEADERS  "src/*.hpp")
file(GLOB_RECURSE SRC "src/*.cpp")

add_executable (PrimeService ${HEADERS} ${SRC})