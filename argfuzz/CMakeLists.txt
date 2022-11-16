set(SOURCES
    main.cpp
)

add_executable(argfuzz ${SOURCES})
target_link_libraries(argfuzz lua_shared)
install(TARGETS argfuzz DESTINATION ./ OPTIONAL)
set_target_properties(argfuzz PROPERTIES INSTALL_RPATH "$ORIGIN/${GMOD_LUASHARED_DIR}")

# FIXME: Port argfuzz to Windows
if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    set_target_properties(argfuzz PROPERTIES EXCLUDE_FROM_ALL TRUE)
endif()
