cmake_minimum_required(VERSION 3.22)

# Если не были переданы аргументы, то задаются стандартные значения
if(NOT DEFINED VERBOSE)
    set(VERBOSE True)
endif()
if(NOT DEFINED MXSRCLIB_ARCH)
    set(MXSRCLIB_ARCH "none")
endif()
if(NOT DEFINED MXSRCLIB_CONVERT_SYMBOL_ENCODING)
    message(FATAL_ERROR "Uninitialized MXSRCLIB_CONVERT_SYMBOL_ENCODING variable.")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Compiler must be selected.")
endif()

set(MXSRCLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Проверка на допустимое значение переменной MXSRCLIB_ARCH
if(NOT (MXSRCLIB_ARCH STREQUAL "none") AND NOT (MXSRCLIB_ARCH STREQUAL "arm") AND
        NOT (MXSRCLIB_ARCH STREQUAL "arm_st_hal") AND NOT (MXSRCLIB_ARCH STREQUAL "avr") AND
        NOT (MXSRCLIB_ARCH STREQUAL "cbuilder") AND NOT (MXSRCLIB_ARCH STREQUAL "dos32") AND
        NOT (MXSRCLIB_ARCH STREQUAL "linux") AND NOT (MXSRCLIB_ARCH STREQUAL "qt") AND
        NOT (MXSRCLIB_ARCH STREQUAL "visualcpp"))
    message(FATAL_ERROR "Invalid mxsrclib arch. For valid values refer to documentation.")
else()
    message(STATUS "MXSRCLIB_ARCH: ${MXSRCLIB_ARCH}")
endif()

# Проверка на допустимое значение переменной MXSRCLIB_CONVERT_SYMBOL_ENCODING
if(NOT (MXSRCLIB_CONVERT_SYMBOL_ENCODING STREQUAL "CP1251") AND
        NOT (MXSRCLIB_CONVERT_SYMBOL_ENCODING STREQUAL "UTF-8"))
    message(FATAL_ERROR "Invalid mxsrclib symbol encoding. For valid values refer to documentation.")
else()
    message(STATUS "MXSRCLIB_CONVERT_SYMBOL_ENCODING: ${MXSRCLIB_CONVERT_SYMBOL_ENCODING}")
endif()

# Получение компилятора без абсолютного пути
set(CXX_COMPILER ${CMAKE_CXX_COMPILER})
string(FIND ${CMAKE_CXX_COMPILER} "/" INDEX_START REVERSE)
if(NOT (INDEX EQUAL -1))
    math(EXPR INDEX_START "${INDEX_START}+1")
    string(SUBSTRING ${CMAKE_CXX_COMPILER} ${INDEX_START} -1 CXX_COMPILER)
endif()

if(${VERBOSE})
    message(STATUS "Using compiler: ${CXX_COMPILER}")
endif()

# Подключение архитектурных директорий (если переменная MXSRCLIB_ARCH не равна "none")
set(ORIGIN_HEADERS_FILE_LIST_arch "")
set(ORIGIN_HEADERS_PATH_LIST_arch "")
if(NOT (MXSRCLIB_ARCH STREQUAL "none"))
    set(ORIGIN_HEADERS_PATH_LIST_arch "${MXSRCLIB_DIR}/arch/${MXSRCLIB_ARCH}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILE_LIST_arch "${ORIGIN_HEADERS_PATH_LIST_arch}/*.h")
    # Для корректной работы архитектуры srm_st_hal необходимо подключить arm архитектуру
    if(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
        file(GLOB_RECURSE ORIGIN_HEADERS_FILE_LIST_arch_arm "${MXSRCLIB_DIR}/arch/arm/*.h")
        list(APPEND ORIGIN_HEADERS_FILE_LIST_arch ORIGIN_HEADERS_FILE_LIST_arch_arm)
        list(APPEND ORIGIN_HEADERS_PATH_LIST_arch "${MXSRCLIB_DIR}/arch/arm")
    endif()
endif()

set(MXSRCLIB_SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
set(MXSRCLIB_HEADERS
        .
        ${ORIGIN_HEADERS_PATH_LIST_arch}
)

# Если выбран кросс-компилятор, то попытаться сконвертировать как статическую библиотеку,
# чтобы Clion не использовал тесты для ПК компиляторов
if(CXX_COMPILER STREQUAL "arm-none-eabi-g++")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
project(mxsrclib)

add_library(mxsrclib STATIC ${MXSRCLIB_SOURCES})
target_include_directories(mxsrclib PRIVATE ${MXSRCLIB_HEADERS})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)
#target_link_libraries(mxsrclib stm32cubef7)

# Выбор флагов компиляции в соответствии с компилятором
if(CXX_COMPILER STREQUAL "g++" OR CXX_COMPILER STREQUAL "arm-none-eabi-g++")
    set(CXX_COMPILER_FLAGS "-finput-charset=CP1251")
else()
    message(FATAL_ERROR "Unsupported compiler.")
endif()
target_compile_options(mxsrclib PRIVATE ${CXX_COMPILER_FLAGS})

# Подключение сконвертированных (если выбран режим конвертации) заголовков
if(NOT (MXSRCLIB_CONVERT_SYMBOL_ENCODING STREQUAL "CP1251"))
    add_subdirectory(convert_mxsrclib_headers)
    get_target_property(CONVERTED_MXSRCLIB_HEADERS_DIR convert_mxsrclib_headers CONVERTED_HEADERS_DIR)
else()
    set(CONVERTED_MXSRCLIB_HEADERS_DIR ${MXSRCLIB_HEADERS})
endif()
target_include_directories(mxsrclib INTERFACE ${CONVERTED_MXSRCLIB_HEADERS_DIR})