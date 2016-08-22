cmake_minimum_required(VERSION 3.0.0)

set(ELE_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/../webthree-helpers/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${ELE_CMAKE_DIR})

# set cmake_policies
include(ElePolicy)
ele_policy()

# project name and version should be set after cmake_policy CMP0048
project(elementrem VERSION "1.2.9")

include(EleCompilerSettings)

include(EleExecutableHelper)

include(EleDependencies)

include(EleUtils)

include(EleOptions)
configure_project(CPUID CURL VMTRACE ROCKSDB OLYMPIC PARANOID TESTS ELHASHCL EVMJIT FATDB)

if (EVMJIT)
	if (DEFINED MSVC AND NOT DEFINED LLVM_DIR)
		set(LLVM_DIR "${ELE_CMAKE_DIR}/extdep/install/windows/x64/share/llvm/cmake")
	endif()
	add_subdirectory(evmjit)
	if (DEFINED MSVC)
		set(EVMJIT_DLLS_LOCAL $<TARGET_FILE:evmjit>)
		set(EVMJIT_DLLS optimized ${EVMJIT_DLLS_LOCAL} debug ${EVMJIT_DLLS_LOCAL} PARENT_SCOPE)
	endif()

endif()

# core libraries
if (NOT EMSCRIPTEN)
	add_subdirectory(libelecore)
	add_subdirectory(libevm)
	add_subdirectory(libelementrem)

	add_subdirectory(libelhash)
	add_subdirectory(libelhashseal)
endif()
add_subdirectory(libevmcore)

if (ELHASHCL)
	add_subdirectory(libelhash-cl)
endif()

# other libraries
add_subdirectory(libnatspec)
if (NOT EMSCRIPTEN)
	add_subdirectory(libtestutils)
endif()

# executables

if (TOOLS)
	add_subdirectory(elekey)
	add_subdirectory(eleminer)
	add_subdirectory(elevm)
endif()

if (TESTS)
	# test
	add_subdirectory(test)
endif()
