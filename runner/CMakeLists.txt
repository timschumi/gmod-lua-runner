set(SOURCES
    main.cpp
)

add_executable(runner ${SOURCES})
target_link_libraries(runner lua_shared)
install(TARGETS runner DESTINATION ./)
set_target_properties(runner PROPERTIES INSTALL_RPATH "$ORIGIN/${GMOD_LUASHARED_DIR}")

# Some distributions will actively stub out libpthread with exceptions if it isn't linked here.
# This should also sufficiently emulate the game relying on libpthread for binary modules that
# specify that themselves.
target_link_options(runner PRIVATE "-pthread")
