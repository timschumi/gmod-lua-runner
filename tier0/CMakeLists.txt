set(SOURCES
    dbg.cpp
)

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_library(tier0 STATIC ${SOURCES})
else()
    add_library(tier0 SHARED ${SOURCES})
endif()
target_include_directories(tier0 PUBLIC ./)
install(TARGETS tier0 DESTINATION ./${GMOD_LIBTIER0_DIR})

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set_target_properties(tier0 PROPERTIES PREFIX "")
endif()
