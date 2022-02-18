cmake_minimum_required (VERSION 3.1)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project (wordle)
include(CheckCXXCompilerFlag)


find_package (Threads REQUIRED)
find_package (Boost REQUIRED)
find_package(TBB REQUIRED tbb)


if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR
    CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    CHECK_CXX_COMPILER_FLAG("-stdlib=libstdc++" HAS_FLAG_LIBSTDCPP)
    if (HAS_FLAG_LIBSTDCPP)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -stdlib=libstdc++")
    endif()

    find_package(ParallelSTL)
    add_compile_definitions(_LIBCPP_HAS_PARALLEL_ALGORITHMS)
endif()

file(GLOB_RECURSE Project_SOURCES "src/*.cpp")
file(GLOB_RECURSE Project_HEADERS "*.h")
set (Project_INCLUDE_DIRS "")

foreach (_headerFile ${Project_HEADERS})
    get_filename_component(_dir ${_headerFile} PATH)
    list (APPEND Project_INCLUDE_DIRS ${_dir})
endforeach()

list(REMOVE_DUPLICATES Project_INCLUDE_DIRS)

include_directories(${Project_INCLUDE_DIRS}
                    "src"
                    "game"
                    "utils"
                    ${Boost_INCLUDE_DIRS})

add_executable(wordlebot
               ${Project_SOURCES})

message("TBB_IMPORTED_TARGETS: ${TBB_IMPORTED_TARGETS}")
target_link_libraries(wordlebot
                      TBB::tbb)

add_executable(wordlegame
    "game/wordlegame.cpp"
    )


CHECK_CXX_COMPILER_FLAG("-march=native" HAS_FLAG_MARCH_NATIVE)
if (HAS_FLAG_MARCH_NATIVE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -march=native")
endif()

CHECK_CXX_COMPILER_FLAG("-mtune=native" HAS_FLAG_MTUNE_NATIVE)
if (HAS_FLAG_MTUNE_NATIVE)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -mtune=native")
endif()

CHECK_CXX_COMPILER_FLAG("-pthread" HAS_FLAG_PTHREAD)
if (HAS_FLAG_PTHREAD)
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -pthread")
endif()


if (APPLE)
    CHECK_CXX_COMPILER_FLAG("-mmacosx-version-min=10.6" HAS_FLAG_MACOSX_VERSION_MIN)
    if (HAS_FLAG_MACOSX_VERSION_MIN)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mmacosx-version-min=10.6")
    endif()
endif()
