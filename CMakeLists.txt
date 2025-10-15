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

set(MXSRCLIB_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Проверка наличия файла irsconfig.h
if (NOT EXISTS "${MXSRCLIB_DIR}/irsconfig.h")
    file(COPY_FILE "${MXSRCLIB_DIR}/irsconfig0.h" "${MXSRCLIB_DIR}/irsconfig.h")
endif()

# Подключение необходимых архитектур
set(RELATIVE_PATH_LIST_arch "arch")
if(MXSRCLIB_ARCH STREQUAL "arm")
    set(RELATIVE_PATH_LIST_arch "${RELATIVE_PATH_LIST_arch}/${MXSRCLIB_ARCH}")
    set(ORIGIN_HEADERS_DIR_LIST_arch "${MXSRCLIB_DIR}/${RELATIVE_PATH_LIST_arch}")
    file(GLOB_RECURSE ORIGIN_HEADERS_FILES_LIST_arch "${ORIGIN_HEADERS_DIR_LIST_arch}/*.h")
    set(ARCH_SOURCES
            arch/${MXSRCLIB_ARCH}/armadc.cpp
            arch/${MXSRCLIB_ARCH}/armcfg.cpp
            arch/${MXSRCLIB_ARCH}/armspi.cpp
            arch/${MXSRCLIB_ARCH}/irsarchint.cpp
            arch/${MXSRCLIB_ARCH}/irsstm32_usb_hid_hardflow.cpp
    )

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

if(${VERBOSE})
    message(STATUS "Используемый компилятор: ${CMAKE_CXX_COMPILER}")
endif()

set(MXSRCLIB_SOURCES
        hardflowg.cpp
        irsadc.cpp
        irsalg.cpp
        irscpu.cpp
        irsdev.cpp
        irsdsp.cpp
        irserror.cpp
        irsfunnel.cpp
        irsgpio.cpp
        irsint.cpp
        irskbd.cpp
        irslocale.cpp
        irsmath.cpp
        irsmbus.cpp
        irsmem.cpp
        irsstring.cpp
        irsstrm.cpp
        timer.cpp
        irsstd.cpp
        irsstd.h
        irsmenu.cpp
        irsmenu.h
        irstime.cpp
        irstime.h
        irsfs.h
        simple_ftp_server.cpp
        simple_ftp_server.h
        irs_param_list.cpp
        irs_param_list.h
        arch/${MXSRCLIB_ARCH}/counter.cpp
        ${ARCH_SOURCES}
)
set(MXSRCLIB_HEADERS_DIR
        .
        ${ORIGIN_HEADERS_DIR_LIST_arch}
)

project(mxsrclib)
add_library(mxsrclib STATIC ${MXSRCLIB_SOURCES})
target_include_directories(mxsrclib PRIVATE ${MXSRCLIB_HEADERS_DIR})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)

if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(COMPILER_OPTIONS
            "-finput-charset=CP1251"
    )

    set(WARNING_OPTIONS
#            "-Werror"
            "-Wextra"
            "-Wall"
            # Только потому что баг с GCC diagnostics ignored "-Wdeprecated" пофиксили только в gcc 13
            # Проблемный варнинг возникает при подключении <strstream>
            "-Wno-deprecated"
    )
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "IAR")
else()
    message(FATAL_ERROR "Нужно дописать опции предупреждений для компилятора")
endif()
target_compile_options(mxsrclib PRIVATE "${COMPILER_OPTIONS}" "${WARNING_OPTIONS}")

# Добавление зависимостей
if(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
    if(TARGET ${MXSRCLIB_STM32CUBE_TARGET})
        target_link_libraries(mxsrclib ${MXSRCLIB_STM32CUBE_TARGET})
    else()
        message(FATAL_ERROR "Не указан таргет с библиотекой STM32Cube.")
    endif()
elseif (MXSRCLIB_ARCH STREQUAL "qt")
    if (CMAKE_SYSTEM_NAME STREQUAL "CYGWIN" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(mxsrclib INTERFACE
                psapi
                ws2_32
        )
    endif ()
endif()

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
