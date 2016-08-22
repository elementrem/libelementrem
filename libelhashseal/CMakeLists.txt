aux_source_directory(. SRC_LIST)

set(EXECUTABLE elhashseal)

file(GLOB HEADERS "*.h")

add_library(${EXECUTABLE} ${SRC_LIST} ${HEADERS})
ele_use(${EXECUTABLE} REQUIRED Ele::elementrem Ele::elecore Ele::elhash Dev::devcrypto Dev::buildinfo)
ele_use(${EXECUTABLE} OPTIONAL Ele::elhash-cl Cpuid)

install( TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
install( FILES ${HEADERS} DESTINATION include/${EXECUTABLE} )

