cmake_minimum_required(VERSION 3.15)

project(fsig)

set(CMAKE_CXX_STANDARD 11)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

set(FSIG_SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/src/main.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/command_line.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/worker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file_reader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/src/file_writer.cpp
)

set(FSIG_INCLUDE_DIRECTORIES
    ${CMAKE_CURRENT_LIST_DIR}/include
)

set(FSIG_COMPILE_DEFINITIONS
    FSIG_VERSION="1.0.0"
    FSIG_DEFAULT_HASH_ALGO="MD5"
)

set(FSIG_COMPILE_OPTIONS
)

set(FSIG_LINK_OPTIONS
)

SET(FSIG_LINK_LIBRARIES
    ${CONAN_LIBS}
)

if(MINGW OR UNIX)
    list(APPEND FSIG_COMPILE_OPTIONS
        # enable diagnostic warnings in project
        -Wall
        -Wextra
        -Wpedantic
        -Weffc++
        -Wcast-align
        -Wcast-qual
        -Wconversion
        -Wctor-dtor-privacy
        -Wenum-compare
        -Wfloat-equal
        -Wnon-virtual-dtor
        -Wold-style-cast
        -Woverloaded-virtual
        -Wredundant-decls
        -Wsign-conversion
        -Wsign-promo
        # disable warnings in boost library
        -isystem ${CONAN_INCLUDE_DIRS_BOOST}
        # disable RTTI
        -fno-rtti
    )
    if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
        list(APPEND FSIG_LINK_OPTIONS
            -s # remove all symbols from executable
        )
    endif()
endif()

if(UNIX)
    list(APPEND FSIG_LINK_OPTIONS
         -static-libgcc
         -static-libstdc++
    )
endif()

if(MINGW)
    list(APPEND FSIG_LINK_OPTIONS
        -static # static link with all mingw libs
    )
    # for boost asio
    # list(APPEND FSIG_LINK_LIBRARIES ws2_32)
endif()

if(MSVC)
    set(COMPILER_FLAGS
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE
        CMAKE_CXX_FLAGS_RELWITHDEBINFO
        CMAKE_C_FLAGS
        CMAKE_C_FLAGS_DEBUG
        CMAKE_C_FLAGS_RELEASE
        CMAKE_C_FLAGS_RELWITHDEBINFO
    )
    foreach(X ${COMPILER_FLAGS})
        # static link with MSVC runtime
        string(REPLACE "/MD" "/MT" ${X} "${${X}}")
        # enable more diagnostic warnings
        string(REPLACE "/W3" "/W4" ${X} "${${X}}")
    endforeach()
    list(APPEND FSIG_COMPILE_DEFINITIONS
        NOMINMAX                # undef MIN MAX WINDOWS macroses
        _CRT_SECURE_NO_WARNINGS # memcpy without warning
        _WIN32_WINNT=0x0601     # Windows 7 or later
        _SILENCE_ALL_CXX17_DEPRECATION_WARNINGS
    )
endif()

add_executable(${PROJECT_NAME}
    ${FSIG_SOURCES}
)
target_include_directories(${PROJECT_NAME}
    PRIVATE ${FSIG_INCLUDE_DIRECTORIES}
)
target_compile_definitions(${PROJECT_NAME}
    PRIVATE ${FSIG_COMPILE_DEFINITIONS}
)
target_compile_options(${PROJECT_NAME}
    PRIVATE ${FSIG_COMPILE_OPTIONS}
)
target_link_options(${PROJECT_NAME}
    PRIVATE ${FSIG_LINK_OPTIONS}
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE ${FSIG_LINK_LIBRARIES}
)

add_executable(${PROJECT_NAME}_test
    ${CMAKE_CURRENT_LIST_DIR}/src/worker.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/helpers.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/memory_reader.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/memory_writer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/test/worker_test.cpp
)
target_include_directories(${PROJECT_NAME}_test
    PRIVATE ${FSIG_INCLUDE_DIRECTORIES} ${CMAKE_CURRENT_LIST_DIR}/test
)
target_compile_definitions(${PROJECT_NAME}_test
    PRIVATE ${FSIG_COMPILE_DEFINITIONS}
)
target_compile_options(${PROJECT_NAME}_test
    PRIVATE ${FSIG_COMPILE_OPTIONS}
)
target_link_options(${PROJECT_NAME}_test
    PRIVATE ${FSIG_LINK_OPTIONS}
)
target_link_libraries(${PROJECT_NAME}_test
    PRIVATE ${FSIG_LINK_LIBRARIES}
)
enable_testing()
add_test(${PROJECT_NAME}_test bin/${PROJECT_NAME}_test)
