cmake_minimum_required(VERSION 2.8)
project(Enet)
if (MSVC)
	# Exception handler model.
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS -DWIN32)
	
	set(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
else (MSVC)
	set(CMAKE_CXX_FLAGS "-std=c++11")
	add_definitions(-DHAS_SOCKLEN_T)
endif (MSVC)

set(SOURCES_ENET
	src/callbacks.c
	src/enet/callbacks.h
	src/compress.c
	src/enet/enet.h
	src/host.c
	src/list.c
	src/enet/list.h
	src/packet.c
	src/peer.c
	src/protocol.c
	src/enet/protocol.h
	src/enet/time.h
	src/enet/types.h
	src/unix.c
	src/enet/unix.h
	src/enet/utility.h
	src/win32.c
	src/enet/win32.h
)
# End of source files.

include_directories(src)

add_library(Enet ${SOURCES_ENET})
