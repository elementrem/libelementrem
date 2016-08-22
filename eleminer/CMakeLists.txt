aux_source_directory(. SRC_LIST)

set(EXECUTABLE eleminer)

file(GLOB HEADERS "*.h")

ele_simple_add_executable(${EXECUTABLE} ${SRC_LIST} ${HEADERS})

ele_use(${EXECUTABLE} REQUIRED JsonRpc::Client Ele::elhashseal)

jsonrpcstub_client_create(minerfarm.json FarmClient ${CMAKE_CURRENT_SOURCE_DIR} FarmClient)
jsonrpcstub_client_create(minerphonehome.json PhoneHome ${CMAKE_CURRENT_SOURCE_DIR} PhoneHome)

if (APPLE)
	install(TARGETS ${EXECUTABLE} DESTINATION bin)
else()
	ele_install_executable(${EXECUTABLE})
endif()
