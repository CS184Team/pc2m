cmake_minimum_required(VERSION 3.0)
project(PointCloud2Mesh)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB LIB_SOURCES src/*.cpp src/*.c)
file(GLOB LIB_HEADERS src/*.h)
add_library(MyLib ${LIB_SOURCES} ${LIB_HEADERS})

file(GLOB APP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/src/ src/*.c src/*.cpp)
foreach(target_file ${APP_SOURCES})
    # message(AUTHOR_WARNING ${target_file})
    string(REPLACE ".cpp" "" target_name ${target_file})
    string(REPLACE ".c" "" target_name ${target_name})
    add_executable(${target_name} src/${target_file})
    target_link_libraries(${target_name}
            MyLib
            )
    install(TARGETS ${target_name} DESTINATION ${PointCloud2Mesh_SOURCE_DIR})
    if (APPLE)
        set_property(TARGET ${target_name} APPEND_STRING PROPERTY COMPILE_FLAGS
                "-Wno-deprecated-declarations -Wno-c++11-extensions")
    endif (APPLE)
endforeach(target_file ${APP_SOURCES})
set(EXECUTABLE_OUTPUT_PATH .)