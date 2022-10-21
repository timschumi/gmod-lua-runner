cmake_minimum_required(VERSION 3.10)
project(gmod-lua-runner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(gmod-module-base/include)
add_subdirectory(lua_shared)
add_subdirectory(luajit)
add_subdirectory(runner)
add_subdirectory(tier0)
