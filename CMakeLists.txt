cmake_minimum_required(VERSION 3.1)
project(9cc)

SET(SRC
  ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/parse.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/codegen.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/token.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/src/util.cpp
  )

add_executable(9cc ${SRC})
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=gnu++17")

add_subdirectory(test)
