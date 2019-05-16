cmake_minimum_required(VERSION 3.10)
project(reflex)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH "cmake")
find_package(Boost REQUIRED system coroutine)

file(GLOB SRC_CPP "app/*.cpp" "app/controller/*.cpp" "app/domain/*.cpp" "app/repository/*.cpp" )
add_executable(reflex main.cpp ${SRC_CPP})

if(CMAKE_C_PLATFORM_ID STREQUAL "Darwin")
    set(DEPENDEICIES ${Boost_LIBRARIES})
else()
    set(DEPENDEICIES ${Boost_LIBRARIES} pthread)
endif()

target_link_libraries(reflex ${DEPENDEICIES})