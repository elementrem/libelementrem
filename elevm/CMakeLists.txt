aux_source_directory(. SRC_LIST)

set(EXECUTABLE elevm)

add_executable(${EXECUTABLE} ${SRC_LIST})

ele_use(${EXECUTABLE} REQUIRED Ele::elementrem Ele::elhashseal)

if (APPLE)
	install(TARGETS ${EXECUTABLE} DESTINATION bin)
else()
	ele_install_executable(${EXECUTABLE})
endif()
