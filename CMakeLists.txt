cmake_minimum_required(VERSION 2.8)
project(MwEnet)
if (MSVC)
	# Exception handler model.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
	
	set(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
else (MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
set(SOURCES_MW
	src/mw/client.cpp
	src/mw/enetnetwork.cpp
	src/mw/enetnetwork.h
	src/mw/network.h
	src/mw/packet.h
	src/mw/server.cpp
	src/mw/server.h
	src/mw/localnetwork.cpp
	src/mw/localnetwork.h
)
# End of source files.

add_subdirectory(Enet)

include_directories(Enet/src)

add_library(MwEnet Enet ${SOURCES_MW})
