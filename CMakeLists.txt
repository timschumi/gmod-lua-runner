cmake_minimum_required(VERSION 3.10)
project(gmod-lua-runner LANGUAGES CXX)

set(LUA_RUNNER_VERSION "dev" CACHE STRING "The user-visible lua-runner version")

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FIXME: Figure out how to handle this properly.
if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set(CMAKE_EXE_LINKER_FLAGS "-static -static-libgcc -static-libstdc++")
endif()

add_subdirectory(argfuzz)
add_subdirectory(gmod-module-base/include)
add_subdirectory(lua_shared)
add_subdirectory(luajit)
add_subdirectory(runner)
add_subdirectory(tier0)

set(CPACK_GENERATOR "TGZ")
set(CPACK_PACKAGE_VERSION "${LUA_RUNNER_VERSION}")
include(CPack)
