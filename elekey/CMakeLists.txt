aux_source_directory(. SRC_LIST)

set(EXECUTABLE elekey)

file(GLOB HEADERS "*.h")

add_executable(${EXECUTABLE} ${SRC_LIST} ${HEADERS})

ele_use(${EXECUTABLE} REQUIRED Dev::devcore Ele::elecore Ele::devcrypto)

if (APPLE)
	install(TARGETS ${EXECUTABLE} DESTINATION bin)
else()
	ele_install_executable(${EXECUTABLE})
endif()
