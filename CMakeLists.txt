cmake_minimum_required(VERSION 3.22)
project(mxsrclib)
set(SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
add_subdirectory(convert_mxsrclib_headers)
get_target_property(CONVERTED_MXSRCLIB_HEADERS_DIR convert_mxsrclib_headers CONVERTED_HEADERS_DIR)
set(INCLUDE_DIRS
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/arm
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/arm_st_hal
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/avr
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/cbuilder
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/dos32
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/linux
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/qt
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/visualcpp
)
set(CMAKE_C_COMPILER arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER arm-none-eabi-g++)
add_library(mxsrclib STATIC ${SOURCES})

add_dependencies(mxsrclib convert_mxsrclib_headers)
target_include_directories(mxsrclib PUBLIC ${INCLUDE_DIRS})

set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)
target_link_libraries(mxsrclib stm32cubef7)
target_compile_options(mxsrclib PRIVATE
        -finput-charset=CP1251
        -Wno-deprecated
)
target_include_directories(mxsrclib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/arch/arm
        ${CMAKE_CURRENT_SOURCE_DIR}/arch/arm_st_hal
)
