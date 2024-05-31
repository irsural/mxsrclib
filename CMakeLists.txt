cmake_minimum_required(VERSION 3.22)
project(mxsrclib)

# Если не были переданы аргументы, то задаются стандартные значения
if(NOT DEFINED VERBOSE)
    set(VERBOSE False)
endif()
if(NOT DEFINED MXSRCLIB_ARCH)
    set (MXSRCLIB_ARCH "none")
endif()
if(NOT DEFINED MXSRCLIB_CONVERT_SYMBOL_ENCODING)
    message(FATAL_ERROR "Uninitialized MXSRCLIB_CONVERT_SYMBOL_ENCODING variable.")
endif()

# Проверка на допустимое значение переменных
if(NOT (MXSRCLIB_ARCH STREQUAL "none") AND NOT (MXSRCLIB_ARCH STREQUAL "arm") AND
        NOT (MXSRCLIB_ARCH STREQUAL "arm_st_hal") AND NOT (MXSRCLIB_ARCH STREQUAL "avr") AND
        NOT (MXSRCLIB_ARCH STREQUAL "cbuilder") AND NOT (MXSRCLIB_ARCH STREQUAL "dos32") AND
        NOT (MXSRCLIB_ARCH STREQUAL "linux") AND NOT (MXSRCLIB_ARCH STREQUAL "qt") AND
        NOT (MXSRCLIB_ARCH STREQUAL "visualcpp"))
    message(FATAL_ERROR "Invalid mxsrclib arch. For valid values refer to documentation.")
else()
    message(STATUS "MXSRCLIB_ARCH: ${MXSRCLIB_ARCH}")
endif()

if(NOT (MXSRCLIB_CONVERT_SYMBOL_ENCODING STREQUAL "cp1251") AND
        NOT (MXSRCLIB_CONVERT_SYMBOL_ENCODING STREQUAL "utf8"))
    message(FATAL_ERROR "Invalid mxsrclib symbol encoding. For valid values refer to documentation.")
else()
    message(STATUS "MXSRCLIB_CONVERT_SYMBOL_ENCODING: ${MXSRCLIB_CONVERT_SYMBOL_ENCODING}")
endif()
set(ORIGIN_HEADER_PATH_LIST_arch "${MXSRCLIB_DIR}/arch/${MXSRCLIB_ARCH}")
file(GLOB_RECURSE ORIGIN_HEADER_FILE_LIST_arch "${ORIGIN_HEADER_PATH_LIST_arch}/*.h")

# Для корректной работы архитектуры srm_st_hal необходимо подключить arm архитектуру
if(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
    file(GLOB_RECURSE ORIGIN_HEADER_FILE_LIST_arch_arm "${MXSRCLIB_DIR}/arch/arm/*.h")
    list(APPEND ORIGIN_HEADER_FILE_LIST_arch ORIGIN_HEADER_FILE_LIST_arch_arm)
    list(APPEND ORIGIN_HEADER_PATH_LIST_arch "${MXSRCLIB_DIR}/arch/arm")
endif()

set(SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
set(INCLUDE_DIRS
        .
        ${ORIGIN_HEADER_PATH_LIST_arch}
)
add_library(mxsrclib STATIC ${SOURCES})
target_include_directories(mxsrclib PRIVATE ${INCLUDE_DIRS})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)
target_link_libraries(mxsrclib stm32cubef7)
target_compile_options(mxsrclib PRIVATE
        -finput-charset=CP1251
        -Wno-deprecated
)

# Подключение сконвертированных (если выбран режим конвертации) заголовков
add_subdirectory(convert_mxsrclib_headers)
get_target_property(CONVERTED_MXSRCLIB_HEADERS_DIR convert_mxsrclib_headers CONVERTED_HEADERS_DIR)
target_include_directories(mxsrclib INTERFACE ${CONVERTED_MXSRCLIB_HEADERS_DIR})