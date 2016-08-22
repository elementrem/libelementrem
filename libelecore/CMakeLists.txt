aux_source_directory(. SRC_LIST)

set(EXECUTABLE elecore)

file(GLOB HEADERS "*.h")

include_directories(BEFORE ..)
add_library(${EXECUTABLE} ${SRC_LIST} ${HEADERS})

ele_use(${EXECUTABLE} REQUIRED Dev::devcrypto Dev::buildinfo Dev::devcore)

install( TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
install( FILES ${HEADERS} DESTINATION include/${EXECUTABLE} )

