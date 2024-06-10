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
#        correct_alg.cpp
#        csvwork.cpp
#        dbgprn.cpp
        hardflowg.cpp
#        i2c.cpp
#        imp_filt.cpp
#        irs_chart_data.cpp
#        irsadc.cpp
        irsalg.cpp
#        irscalc.cpp
        irscpu.cpp
#        irsdbgutil.cpp
#        irsdcp.cpp
#        irsdev.cpp
        irsdevices.cpp
#        irsdsp.cpp
#        irseeprom.cpp
        irserror.cpp
#        irsexcept.cpp
        irsfilter.cpp
#        irsfunnel.cpp
#        irsgpio.cpp
#        irshfftp.cpp
#        irsinit.cpp
        irsint.cpp
#        irskbd.cpp
        irslocale.cpp
#        irslwip.cpp
#        irsmath.cpp
#        irsmatrix.cpp
        irsmbus.cpp
#        irsmcutil.cpp
#        irsmem.cpp
#        irsmenu.cpp
#        irsprop.cpp
#        irsstd.cpp
        irsstring.cpp
#        irsstrm.cpp
#        irssysutils.cpp
        irstcpip.cpp
        irstime.cpp
        irsvariant.cpp
        measmul.cpp
        meassup.cpp
        mxdata.cpp
#        mxextbase.cpp
#        mxextchart.cpp
        mxifar.cpp
        mxnet.cpp
        mxnetc.cpp
        mxnetr.cpp
        niusbgpib.cpp
        niusbgpib_hardflow.cpp
        timer.cpp
        arch/${MXSRCLIB_ARCH}/counter.cpp
        arch/${MXSRCLIB_ARCH}/mxifa.cpp
)
set(MXSRCLIB_HEADERS_DIR
        .
        ${IRSCONFIG_DIR}
        ${ORIGIN_HEADERS_DIR_LIST_arch}
)

# Если выбран кросс-компилятор, то попытаться скомпилировать как статическую библиотеку,
# чтобы Clion не использовал тесты для ПК компиляторов
if(CMAKE_CXX_COMPILER MATCHES "^.*arm-none-eabi-g\\+\\+$")
    set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
endif()
project(mxsrclib)

add_library(mxsrclib STATIC ${MXSRCLIB_SOURCES})
target_include_directories(mxsrclib PRIVATE ${MXSRCLIB_HEADERS_DIR})
set_target_properties(mxsrclib PROPERTIES OUTPUT_NAME mxsrclib)

# Выбор флагов компиляции в соответствии с компилятором
if(CMAKE_CXX_COMPILER MATCHES "^.*g\\+\\+")
    set(CXX_COMPILER_FLAGS "-finput-charset=CP1251")
else()
    message(FATAL_ERROR "Компилятор не поддерживается.")
endif()
target_compile_options(mxsrclib PRIVATE ${CXX_COMPILER_FLAGS})

# Добавление зависимостей
if(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
    if(TARGET ${MXSRCLIB_STM32CUBE_TARGET})
        target_link_libraries(mxsrclib ${MXSRCLIB_STM32CUBE_TARGET})
    else()
        message(FATAL_ERROR "Не указан таргет с библиотекой STM32Cube.")
    endif()
elseif (MXSRCLIB_ARCH STREQUAL "qt")
    if (CMAKE_SYSTEM_NAME STREQUAL "CYGWIN" OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
        target_link_libraries(mxsrclib -lpsapi -lws2_32 -static)
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
