CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
PROJECT(MwEnet)
if (MSVC)
	# Exception handler model.
	ADD_DEFINITIONS(-D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)
	ADD_DEFINITIONS(-DWIN32)
	
	SET(WINDOWS_ENET_DEPENDENCIES "ws2_32;winmm")
else (MSVC)
	SET(CMAKE_CXX_FLAGS "-std=c++11")
endif (MSVC)

# Source files.
SET(SOURCES_MW
	mw/enet/enetnetwork.cpp
	mw/enet/client.cpp
	mw/enet/server.cpp
	mw/enet/enetnetwork.h
	mw/enet/client.h
	mw/enet/server.h
)
SET(SOURCES_ENET
	enet-1.3.9/enet/callbacks.c
	enet-1.3.9/enet/callbacks.h
	enet-1.3.9/enet/compress.c
	enet-1.3.9/enet/enet.h
	enet-1.3.9/enet/host.c
	enet-1.3.9/enet/list.c
	enet-1.3.9/enet/list.h
	enet-1.3.9/enet/packet.c
	enet-1.3.9/enet/peer.c
	enet-1.3.9/enet/protocol.c
	enet-1.3.9/enet/protocol.h
	enet-1.3.9/enet/time.h
	enet-1.3.9/enet/types.h
	enet-1.3.9/enet/unix.c
	enet-1.3.9/enet/unix.h
	enet-1.3.9/enet/utility.h
	enet-1.3.9/enet/win32.c
	enet-1.3.9/enet/win32.h
)
# End of source files.

ADD_SUBDIRECTORY(MwExtra)
INCLUDE_DIRECTORIES("MwExtra/src" "enet-1.3.9")
ADD_LIBRARY(MwEnet STATIC ${SOURCES_MW} ${SOURCES_ENET})
