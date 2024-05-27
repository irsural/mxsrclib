project(mxsrclib)
set(SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)

set(HEADERS
        timer.h
        irscpp.h
        arch/arm_st_hal/counter.h
)
add_library(mxsrclib STATIC ${SOURCES} ${HEADERS})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME libmxsrclib)
target_link_libraries(mxsrclib -static)
target_compile_options(mxsrclib PRIVATE
        -finput-charset=CP1251
        -fPIC # Нужно для включения в dll
        -Wno-deprecated
        -Wextra
        -Woverloaded-virtual
        -Wno-unused-parameter
        -Wno-unused-local-typedefs
        -Wno-misleading-indentation
        -Wno-unused-variable)
target_include_directories(mxsrclib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/arch/arm_st_hal)