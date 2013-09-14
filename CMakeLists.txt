#SET(CMAKE_C_COMPILER "clang")
#SET(CMAKE_CXX_COMPILER "g++")

cmake_minimum_required(VERSION 2.6.1)
PROJECT(BigInt)

#find_package()

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_BINARY_DIR})

FILE(GLOB_RECURSE bigint_SOURCES *.cc)
FILE(GLOB_RECURSE bigint_HEADERS *.hpp)

if (WIN32)
    SET(USRDIR "./")
endif (WIN32)
if (UNIX)
    SET(USRDIR "${CMAKE_INSTALL_PREFIX}/share/")
endif (UNIX)

#ADD_DEFINITIONS(-D__PALETTES_PATH__=${USRDIR}/palettes)
#ADD_DEFINITIONS(-D__PALETTES_PATH__=palettes)

SET(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -std=c++11")
SET(CMAKE_CXX_FLAGS_RELEASE "-g -pipe -Ofast -march=native -Wall -std=c++11 -DNDEBUG")
SET(CMAKE_EXE_LINKER_FLAGS "")

if (DISTCC_REMOTE_BUILD)
    SET(CMAKE_CXX_FLAGS_BASE "-pipe -Ofast -g -Wall -std=c++11 -DNDEBUG")

    SET(CMAKE_CXX_FLAGS_ARCH "")
    EXECUTE_PROCESS(COMMAND ${CMAKE_CXX_COMPILER} -march=native -E -v - 
                    OUTPUT_FILE "/tmp/cmake-gcc.log"
                    ERROR_FILE "/tmp/cmake-gcc.log"
                    INPUT_FILE "/dev/null"
                   )
    EXECUTE_PROCESS(COMMAND grep cc1
                    INPUT_FILE "/tmp/cmake-gcc.log"
                    OUTPUT_VARIABLE CMAKE_CXX_FLAGS_ARCH
                    OUTPUT_FILE "/tmp/cmake-arch.log"
                   )

    EXECUTE_PROCESS(COMMAND sed -re "s/(.*) - (.*)/\\2/"
                    INPUT_FILE "/tmp/cmake-arch.log"
                    OUTPUT_VARIABLE CMAKE_CXX_FLAGS_ARCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE
                   )

    SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_BASE} ${CMAKE_CXX_FLAGS_ARCH}")
    #MESSAGE ("${CMAKE_CXX_FLAGS_RELEASE}")
endif (DISTCC_REMOTE_BUILD)

ADD_EXECUTABLE(test ${bigint_SOURCES}
                    ${bigint_HEADERS}
                )
#TARGET_LINK_LIBRARIES(bigint)
