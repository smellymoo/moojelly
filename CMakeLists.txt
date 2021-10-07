cmake_minimum_required(VERSION 3.4)
set (Moojelly_VERSION_MAJOR 1)
set (Moojelly_VERSION_MINOR 0)
project(Moojelly VERSION ${Moojelly_VERSION_MAJOR}.${Moojelly_VERSION_MINOR} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsign-promo -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wdeprecated -Wfloat-equal")
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
include(${wxWidgets_USE_FILE})

set(ENABLE_TEST OFF CACHE BOOL "Enable Google Test")

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Debug CACHE STRING  "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."       FORCE )
endif()

if( ENABLE_TEST )
    message("Unit test enabled")
    enable_testing()
endif()

set (SRC_FILES "")
set (HEADER_FILES "")

file(GLOB SRC_FILES
    "./*.cpp"
    )

file(GLOB HEADER_FILES
    "./*.h"
    )



add_executable(Moojelly ${SRC_FILES} )
target_include_directories(Moojelly PUBLIC wxWidgets_INCLUDE_DIRS)
target_link_libraries(Moojelly ${wxWidgets_LIBRARIES})