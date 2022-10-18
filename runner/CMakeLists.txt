set(SOURCES
    main.cpp
)

add_executable(runner ${SOURCES})
target_link_libraries(runner lua_shared)
