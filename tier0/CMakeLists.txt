set(SOURCES
    dbg.cpp
)

add_library(tier0 SHARED ${SOURCES})
target_include_directories(tier0 PUBLIC ./)
install(TARGETS tier0 DESTINATION ./bin/)
