cmake_minimum_required(VERSION 3.9)

make_project_name(PROJECT_NAME)
project(${PROJECT_NAME})

message(STATUS "Configure project[${PROJECT_NAME}]: ${CMAKE_CURRENT_SOURCE_DIR}")

find_src(SRC_FILES ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${SRC_FILES})
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SRC_FILES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${INCLUDE_PUBLIC}
    ${CMAKE_BINARY_DIR}/ffmpeg/out/include
)

add_dependencies(${PROJECT_NAME}
    VXFFMpeg
)

target_link_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_BINARY_DIR}/ffmpeg/out/lib
)

target_link_libraries(${PROJECT_NAME}
    avformat
    avcodec
    swscale
    swresample
    avfilter
    avutil
    pthread
)

config_install(${PROJECT_NAME})

install(FILES 
${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
DESTINATION examples)
