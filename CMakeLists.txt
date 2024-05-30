cmake_minimum_required(VERSION 3.22)
project(mxsrclib)
set(SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
set(INCLUDE_DIRS
        .
        arch/arm
        arch/arm_st_hal
        arch/avr
        arch/cbuilder
        arch/dos32
        arch/linux
        arch/qt
        arch/visualcpp
)
add_library(mxsrclib STATIC ${SOURCES})
target_include_directories(mxsrclib PUBLIC ${INCLUDE_DIRS})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)
target_link_libraries(mxsrclib stm32cubef7)
target_compile_options(mxsrclib PRIVATE
        -finput-charset=CP1251
        -Wno-deprecated
)
