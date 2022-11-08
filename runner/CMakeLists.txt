set(SOURCES
    main.cpp
)

add_executable(runner ${SOURCES})
target_link_libraries(runner lua_shared)
install(TARGETS runner DESTINATION ./)
set_target_properties(runner PROPERTIES INSTALL_RPATH "$ORIGIN/${GMOD_LUASHARED_DIR}")
