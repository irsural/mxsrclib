project(mxsrclib)
set(SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
add_library(mxsrclib STATIC ${SOURCES})
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
