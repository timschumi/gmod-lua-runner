set(SOURCES
    main.cpp
)

add_executable(argfuzz ${SOURCES})
target_link_libraries(argfuzz lua_shared)
install(TARGETS argfuzz DESTINATION ./)
set_target_properties(argfuzz PROPERTIES INSTALL_RPATH "$ORIGIN/${GMOD_LUASHARED_DIR}")
