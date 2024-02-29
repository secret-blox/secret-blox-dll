cmake_minimum_required (VERSION 3.8)

option(UNIT_TEST "Perform unit testing" OFF)

if(UNIT_TEST)
    add_definitions(-DUNIT_TEST)
endif()

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Check for Debug build type
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message(FATAL_ERROR "!!! DEBUG BUILD IS NOT ALLOWED !!!")
endif()

# Check for 32-bit architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    message(FATAL_ERROR "!!! 32-BIT ARCHITECTURE IS NOT ALLOWED !!!")
endif()

if(UNIX)
	# cargo install xwin; xwin --accept-license --cache-dir ~/.xwin-cache splat --include-debug-libs --output ~/.xwin-cache/splat

	set(CMAKE_SYSTEM_NAME Windows)
	set(CMAKE_SYSTEM_VERSION 10.0)
	set(CMAKE_SYSTEM_PROCESSOR AMD64)

	set(CRT_INC "/home/user/.xwin-cache/splat/crt/include")
	set(CRT_LIB "/home/user/.xwin-cache/splat/crt/lib")
	set(SDK_INC "/home/user/.xwin-cache/splat/sdk/include")
	set(SDK_LIB "/home/user/.xwin-cache/splat/sdk/lib")

	set(CMAKE_C_COMPILER clang-cl)
	set(CMAKE_CXX_COMPILER clang-cl)
	set(CMAKE_LINKER lld-link)

	set(COMPILE_FLAGS
		--target=x86_64-pc-windows-msvc
		-Wunused-command-line-argument
		-D_CRT_SECURE_NO_WARNINGS
		-fuse-ld=lld-link
		-imsvc "${CRT_INC}"
		-imsvc "${SDK_INC}/ucrt"
		-imsvc "${SDK_INC}/um"
		-imsvc "${SDK_INC}/shared"
	)

	string(REPLACE ";" " " COMPILE_FLAGS "${COMPILE_FLAGS}")
	string(APPEND CMAKE_C_FLAGS_INIT " ${COMPILE_FLAGS}")
	string(APPEND CMAKE_CXX_FLAGS_INIT " ${COMPILE_FLAGS}")

	set(LINK_FLAGS
		/manifest:no
		-libpath:"${CRT_LIB}/x86_64"
		-libpath:"${SDK_LIB}/ucrt/x86_64"
		-libpath:"${SDK_LIB}/um/x86_64"
	)

	string(REPLACE ";" " " LINK_FLAGS "${LINK_FLAGS}")
	string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${LINK_FLAGS}")
	string(APPEND CMAKE_MODULE_LINKER_FLAGS_INIT " ${LINK_FLAGS}")
	string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${LINK_FLAGS}")

	set(CMAKE_C_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)
	set(CMAKE_CXX_STANDARD_LIBRARIES "" CACHE STRING "" FORCE)

	cmake_policy(SET CMP0091 NEW)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif(UNIX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_ASM_NASM_LINK_EXECUTABLE "ld <CMAKE_ASM_NASM_LINK_FLAGS> <LINK_FLAGS> <OBJECTS>  -o <TARGET> <LINK_LIBRARIES>")
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS ${CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS} s S)

project (
  "secret-blox-dll"
  VERSION 0.0.0
  LANGUAGES CXX
)

# Find all source files in src & include directories for Security
file(GLOB_RECURSE SEC_SRC_FILES "Security/src/*.cpp")
file(GLOB_RECURSE SEC_HEADER_FILES "Security/include/*.h" "Security/include/*.hpp")

# Find all source files in src & include directories
file(GLOB_RECURSE SRC_FILES "Rbx/src/*.cpp" "Internal/src/*.cpp" "easywsclient/easywsclient.cpp")
file(GLOB_RECURSE HEADER_FILES "Rbx/include/*.h" "Rbx/include/*.hpp" "Internal/include/*.h" "Internal/include/*.hpp" "easywsclient/easywsclient.hpp")

# Security LIB
add_library(SB.Security STATIC
	${SEC_SRC_FILES} ${SEC_HEADER_FILES}
)
target_include_directories(SB.Security INTERFACE Security/include)

# Generate version information for Windows
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

# Internal DLL
add_library(SB.Internal SHARED
	${SRC_FILES} ${HEADER_FILES}
	${CMAKE_CURRENT_BINARY_DIR}/version.rc 
)
target_include_directories(SB.Internal
    PUBLIC Internal/include
    PUBLIC Rbx/include
	PUBLIC easywsclient
	PUBLIC SB.Security
	PUBLIC Luau.Common
	PUBLIC Luau.Compiler
	PUBLIC Luau/Vm/src
)

# Set fake name nvwgf2um.dll - NVIDIA D3D User Mode Driver
set_target_properties(SB.Internal PROPERTIES
    OUTPUT_NAME "nvwgf2um2"
)
set(CMAKE_ASM_NASM_OBJECT_FORMAT win64)

# Add Luau directory
add_subdirectory(Luau)

# Link dll with libraries
target_link_libraries(SB.Internal PUBLIC SB.Security)
target_link_libraries(SB.Internal PUBLIC Luau.Common)
target_link_libraries(SB.Internal PUBLIC Luau.Compiler)
target_link_libraries(SB.Internal PUBLIC Luau.VM)


# Define the directory containing Lua scripts
set(LUA_SCRIPTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/Scripts)

# Get a list of Lua script files in the directory
file(GLOB LUA_SCRIPT_FILES ${LUA_SCRIPTS_DIR}/*.lua)

# Generate a C++ header file containing the Lua scripts
set(GENERATED_HEADER_SCRIPTS ${CMAKE_CURRENT_BINARY_DIR}/sb_scripts.hpp)
file(WRITE ${GENERATED_HEADER_SCRIPTS} "// Auto-generated header file\n\n")

foreach(SCRIPT_FILE ${LUA_SCRIPT_FILES})
    file(READ ${SCRIPT_FILE} SCRIPT_CONTENTS HEX)
    string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1, " SCRIPT_CONTENTS "${SCRIPT_CONTENTS}")
    file(RELATIVE_PATH SCRIPT_NAME ${LUA_SCRIPTS_DIR} ${SCRIPT_FILE})
    string(REPLACE "/" "_" SCRIPT_NAME ${SCRIPT_NAME})
    string(REGEX REPLACE "\\.lua$" "" SCRIPT_NAME ${SCRIPT_NAME})
	string(TOUPPER ${SCRIPT_NAME} SCRIPT_NAME)
    file(APPEND ${GENERATED_HEADER_SCRIPTS} "static const char SB_SCRIPT_${SCRIPT_NAME}[] = {${SCRIPT_CONTENTS} 0x0};\n\n")
endforeach()

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# external libraries
