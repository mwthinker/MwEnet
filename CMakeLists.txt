cmake_minimum_required(VERSION 2.8)
project(MwEnet)
if (MSVC)
	# Exception handler model.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
	
	set(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
else (MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

add_subdirectory(Enet)

include_directories(Enet/src)


# Source files.
set(SOURCES_MW
	src/mw/enetclient.cpp
	src/mw/enetclient.h
	src/mw/enetnetwork.cpp
	src/mw/enetnetwork.h
	src/mw/network.h
	src/mw/packet.h
	src/mw/enetserver.cpp
	src/mw/enetserver.h
	src/mw/localnetwork.cpp
	src/mw/localnetwork.h
	src/mw/server.h
)
# End of source files.


add_library(MwEnet Enet ${SOURCES_MW})
