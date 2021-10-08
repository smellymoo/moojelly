cmake_minimum_required(VERSION 3.4)
set (Moojelly_VERSION_MAJOR 1)
set (Moojelly_VERSION_MINOR 0)
project(Moojelly VERSION ${Moojelly_VERSION_MAJOR}.${Moojelly_VERSION_MINOR} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
if(CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wsign-promo -Wnon-virtual-dtor -Wctor-dtor-privacy -Woverloaded-virtual -Wold-style-cast -Wpointer-arith -Wshadow -Wunused -Wuninitialized -Winit-self -Wdeprecated -Wfloat-equal")
endif()
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set (BINARY_NAME "Moojelly.exe")
    set(wxWidgets_ROOT_DIR "C:/wxWidgets-3.1.5/")
    set(wxWidgets_LIB_DIR "C:/wxWidgets-3.1.5/lib/vc_x64_lib")
    #target_link_directories("C:/wxWidgets-3.1.5/lib/vc_x64_lib")
    #set(wxWidgets_LIB_DIR "C:/wxWidgets-3.1.5/lib/vc_dll")
    #set(wxWidgets_LIB_DIR "C:/wxWidgets-3.1.5/lib/vc_x64_dll")
    
    find_package(wxWidgets COMPONENTS gl core base REQUIRED)
else()
    find_package(wxWidgets REQUIRED gl core base OPTIONAL_COMPONENTS net)
endif()

message("Libs ${wxWidgets_LIBRARIES}")

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

set (BINARY_NAME "Moojelly")




if(WIN32)
add_executable(${BINARY_NAME} WIN32 ${SRC_FILES} )
else()
add_executable(${BINARY_NAME} ${SRC_FILES} )
endif()
target_include_directories(${BINARY_NAME} PUBLIC wxWidgets_INCLUDE_DIRS)
target_link_libraries(${BINARY_NAME} ${wxWidgets_LIBRARIES})
