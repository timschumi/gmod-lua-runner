if(NOT CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
	set(CMAKE_SYSTEM_NAME Linux)
endif()

set(CMAKE_SYSTEM_PROCESSOR "i686")

set(CPACK_SYSTEM_NAME "linux")
set(GMOD_LIBTIER0_DIR "bin")
set(GMOD_LUASHARED_DIR "garrysmod/bin")
set(GMOD_MODULE_ARCH "linux")