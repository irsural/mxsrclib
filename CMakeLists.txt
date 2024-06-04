cmake_minimum_required(VERSION 3.22)

# Если не были переданы аргументы, то задаются стандартные значения
if(NOT DEFINED VERBOSE)
    set(VERBOSE True)
endif()
if(NOT DEFINED MXSRCLIB_ARCH)
    message(FATAL_ERROR "Необходимо выбрать архитектуру.")
endif()
if(NOT DEFINED MXSRCLIB_UTF8)
    set(MXSRCLIB_UTF8 False)
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    message(FATAL_ERROR "Необходимо выбрать компилятор.")
endif()
if(NOT DEFINED IRSCONFIG_DIR)
    message(FATAL_ERROR "Не указан путь до директории irsconfig.h файла")
endif()

set(MXSRCLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Подключение необходимых архитектур
set(RELATIVE_PATH_LIST_arch "arch")
if(MXSRCLIB_ARCH STREQUAL "arm")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")

    # Архитектуре arm_st_hal необходима архитектура arm
    list(APPEND RELATIVE_PATH_LIST_arch "arch/arm")
    list(APPEND ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/arch/arm")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch_2 "${MXSRCLIB_DIR}/arch/arm/*.h")
    list(APPEND ORIGIN_HEADERS_FILES_LIST_arch ${ORIGIN_HEADERS_FILES_LIST_arch_2})
elseif(MXSRCLIB_ARCH STREQUAL "avr")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "cbuilder")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "dos32")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "linux")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "qt")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
elseif(MXSRCLIB_ARCH STREQUAL "visualcpp")
    message(WARNING "Архитектура ${MXSRCLIB_ARCH} в библиотеке mxsrclib не была протестирована")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
else()
    message(FATAL_ERROR "Недопустимое значение переменной MXSRCLIB_ARCH")
endif()

# Получение компилятора без абсолютного пути
set(CXX_COMPILER ${CMAKE_CXX_COMPILER})
string(FIND ${CMAKE_CXX_COMPILER} "/" INDEX_START REVERSE)
if(NOT (INDEX EQUAL -1))
    math(EXPR INDEX_START "${INDEX_START}+1")
    string(SUBSTRING ${CMAKE_CXX_COMPILER} ${INDEX_START} -1 CXX_COMPILER)
endif()

if(${VERBOSE})
    message(STATUS "Используемый компилятор: ${CXX_COMPILER}")
endif()

set(MXSRCLIB_SOURCES
        timer.cpp
        irscpu.cpp
        arch/arm_st_hal/counter.cpp
)
set(MXSRCLIB_HEADERS_DIR
        .
        ${IRSCONFIG_DIR}
        ${ORIGIN_HEADERS_DIR_LIST_arch}
)

# Если выбран кросс-компилятор, то попытаться скомпилировать как статическую библиотеку,
# чтобы Clion не использовал тесты для ПК компиляторов
if(CXX_COMPILER STREQUAL "arm-none-eabi-g++")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
project(mxsrclib)

add_library(mxsrclib STATIC ${MXSRCLIB_SOURCES})
target_include_directories(mxsrclib PRIVATE ${MXSRCLIB_HEADERS_DIR})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)

# Выбор флагов компиляции в соответствии с компилятором
if(CXX_COMPILER STREQUAL "g++" OR CXX_COMPILER STREQUAL "arm-none-eabi-g++")
    # При кросс-компиляции на arm необходимо достать библиотеки из stm32cube
    target_link_libraries(mxsrclib stm32cubef7)
    set(CXX_COMPILER_FLAGS "-finput-charset=CP1251")
else()
    message(FATAL_ERROR "Компилятор не поддерживается.")
endif()
target_compile_options(mxsrclib PRIVATE ${CXX_COMPILER_FLAGS})

# Подключение сконвертированных (если выбран режим конвертации) заголовков
if(${MXSRCLIB_UTF8})
    add_subdirectory(utf8_headers)
    add_dependencies(mxsrclib utf8_headers)
    get_target_property(CONVERTED_MXSRCLIB_HEADERS_DIR utf8_headers CONVERTED_HEADERS_DIR)
    set(CONVERTED_MXSRCLIB_HEADERS ${CONVERTED_MXSRCLIB_HEADERS_DIR})
    foreach(RELATIVE_PATH_arch ${RELATIVE_PATH_LIST_arch})
        list(APPEND CONVERTED_MXSRCLIB_HEADERS "${CONVERTED_MXSRCLIB_HEADERS_DIR}/${RELATIVE_PATH_arch}")
    endforeach()
    target_include_directories(mxsrclib INTERFACE ${CONVERTED_MXSRCLIB_HEADERS})
else()
    target_include_directories(mxsrclib INTERFACE ${MXSRCLIB_HEADERS_DIR})
endif()
