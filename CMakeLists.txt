cmake_minimum_required(VERSION 3.28)

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

project(
	internal
	VERSION 0.0.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(
	GLOB_RECURSE
	SOURCES
	CONFIGURE_DEPENDS
	"${PROJECT_SOURCE_DIR}/src/*.cpp"
	"${PROJECT_SOURCE_DIR}/src/*.hpp"
)

add_library(${PROJECT_NAME} SHARED ${SOURCES})
