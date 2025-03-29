cmake_minimum_required(VERSION 3.20)

project(YourProgramName CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable optimization flags
if (CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options(/O2)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options(-O2)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
    add_compile_options(-O3)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    add_compile_options(-O3)
endif()

# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.hpp")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE src)

# FFmpeg
if (APPLE)
    set(FFMPEG_DIR_PATH "/usr/local/opt/ffmpeg@5")
elseif (UNIX)
    set(FFMPEG_DIR_PATH "/usr/local")
elseif (WIN32)
    set(FFMPEG_DIR_PATH "C:/ffmpeg")
endif()

include_directories(${FFMPEG_DIR_PATH}/include)
link_directories(${FFMPEG_DIR_PATH}/lib)

set(FFMPEG_LIBS avdevice avcodec avfilter avutil swscale avformat swresample postproc)
foreach(lib ${FFMPEG_LIBS})
    find_library(${lib}_LIB ${lib} PATHS ${FFMPEG_DIR_PATH}/lib NO_DEFAULT_PATH)
    if (${lib}_LIB)
        list(APPEND FFMPEG_LIBRARIES ${${lib}_LIB})
    else()
        message(FATAL_ERROR "Failed to find library: ${lib}")
    endif()
endforeach()

# SDL3
find_package(SDL3 REQUIRED)
target_include_directories(${PROJECT_NAME} PRIVATE ${SDL3_INCLUDE_DIRS})

# Boost
if (APPLE)
    set(BOOST_DIR_PATH "/Users/qix/Desktop/project/code/dependencies1/boost")
elseif (UNIX)
    set(BOOST_DIR_PATH "/usr/local/boost")
elseif (WIN32)
    set(BOOST_DIR_PATH "C:/boost")
endif()

include_directories(${BOOST_DIR_PATH}/include)
file(GLOB BOOST_LIBS "${BOOST_DIR_PATH}/lib/libboost_*.a")
foreach(LIB IN LISTS BOOST_LIBS)
    target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB})
endforeach()

# OpenCV
find_package(OpenCV REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE ${OpenCV_LIBS})

# OpenSSL
if (APPLE)
    set(OPENSSL_DIR_PATH "/usr/local/Cellar/openssl@3/3.4.0")
elseif (UNIX)
    set(OPENSSL_DIR_PATH "/usr/local/openssl")
elseif (WIN32)
    set(OPENSSL_DIR_PATH "C:/openssl")
endif()

include_directories(${OPENSSL_DIR_PATH}/include)
link_directories(${OPENSSL_DIR_PATH}/lib)

set(OPENSSL_LIBS ssl crypto)
foreach(lib ${OPENSSL_LIBS})
    find_library(${lib}_LIB ${lib} PATHS ${OPENSSL_DIR_PATH}/lib NO_DEFAULT_PATH)
    if (${lib}_LIB)
        message(STATUS "Found library: ${lib} at ${${lib}_LIB}")
        list(APPEND OPENSSL_LIBRARIES ${${lib}_LIB})
    else()
        message(FATAL_ERROR "Failed to find library: ${lib}")
    endif()
endforeach()

# breUtils
include_directories("/breUtils")

# Link libraries
target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    ${FFMPEG_LIBRARIES} 
    SDL3::SDL3
    ${OPENSSL_LIBRARIES}
    ${OpenCV_LIBS}
)
