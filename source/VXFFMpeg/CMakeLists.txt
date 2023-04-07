set (PROJECT_NAME VXFFMpeg)
project(${PROJECT_NAME} VERSION 0.0.1)

set(OPTIONS --disable-programs  --disable-avdevice --disable-postproc --enable-shared --disable-doc --disable-static --enable-gpl --enable-libx264 --enable-pic --prefix=out)

if (${CMAKE_SYSTEM_PROCESSOR} STREQUAL "aarch64")
set(OPTIONS ${OPTIONS} --enable-cross-compile --arch=aarch64 --target-os=linux --cross-prefix=${ARMCC_PREFIX})
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
set(OPTIONS ${OPTIONS} --enable-debug=3 --disable-optimizations --disable-stripping)
endif()


message(STATUS "Building Options: ${OPTIONS}")

set(SOURCE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(BUILD_DIR "${CMAKE_BINARY_DIR}/ffmpeg")
set(CONFIGURE_OPTIONS "${OPTIONS}")
set(INST_PREFIX "${BUILD_DIR}/out")

if(NOT EXISTS ${BUILD_DIR}/config.h)
    message(STATUS "configure ffmpeg")
    file(MAKE_DIRECTORY "${BUILD_DIR}")
    execute_process(COMMAND ${SOURCE_PATH}/configure ${OPTIONS} WORKING_DIRECTORY ${BUILD_DIR})
    message(STATUS "configure ffmpeg done")
endif()

add_custom_target(${PROJECT_NAME} ALL
    COMMAND  make install -j8
    WORKING_DIRECTORY ${BUILD_DIR}
    COMMENT "building ffmpeg")