set(EXECUTABLE elhash-cl)

# A custom command and target to turn the OpenCL kernel into a byte array header
# The normal build depends on it properly and if the kernel file is changed, then
# a rebuild of libelhash-cl should be triggered
add_custom_command(
	OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/elhash_cl_miner_kernel.h
	COMMAND ${CMAKE_COMMAND} ARGS
	-DBIN2H_SOURCE_FILE="${CMAKE_CURRENT_SOURCE_DIR}/elhash_cl_miner_kernel.cl"
	-DBIN2H_VARIABLE_NAME=elhash_cl_miner_kernel
	-DBIN2H_HEADER_FILE="${CMAKE_CURRENT_BINARY_DIR}/elhash_cl_miner_kernel.h"
	-P "${CMAKE_CURRENT_SOURCE_DIR}/bin2h.cmake"
	COMMENT "Generating OpenCL Kernel Byte Array"
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/elhash_cl_miner_kernel.cl
)
add_custom_target(clbin2h DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/elhash_cl_miner_kernel.h ${CMAKE_CURRENT_SOURCE_DIR}/elhash_cl_miner_kernel.cl)

aux_source_directory(. SRC_LIST)
file(GLOB OUR_HEADERS "*.h")
set(HEADERS ${OUR_HEADERS} ${CMAKE_CURRENT_BINARY_DIR}/elhash_cl_miner_kernel.h)

# TODO: Should fix properly. Cmake >= 3.4.0 detects a self linking of elhash-cl library here.
cmake_policy(SET CMP0038 OLD)
add_library(${EXECUTABLE} ${SRC_LIST} ${HEADERS})
ele_use(${EXECUTABLE} REQUIRED Ele::elhash OpenCL)
target_include_directories(${EXECUTABLE} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

install( TARGETS ${EXECUTABLE} RUNTIME DESTINATION bin ARCHIVE DESTINATION lib LIBRARY DESTINATION lib )
install( FILES ${HEADERS} DESTINATION include/${EXECUTABLE} )
