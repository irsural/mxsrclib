include(FindIconv)
if (NOT Iconv_FOUND)
    message("Error iconv not found")
endif ()
cmake_path(GET CMAKE_CURRENT_SOURCE_DIR PARENT_PATH SOURCE_DIR)
file(GLOB ORIGIN_HEADER_PATH_LIST "${SOURCE_DIR}/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_arm "${SOURCE_DIR}/arch/arm/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_arm_st_hal "${SOURCE_DIR}/arch/arm_st_hal/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_avr "${SOURCE_DIR}/arch/avr/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_cbuilder "${SOURCE_DIR}/arch/cbuilder/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_dos32 "${SOURCE_DIR}/arch/dos32/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_linux "${SOURCE_DIR}/arch/linux/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_qt "${SOURCE_DIR}/arch/qt/*.h")
file(GLOB_RECURSE ORIGIN_HEADER_PATH_LIST_arch_visualcpp "${SOURCE_DIR}/arch/visualcpp/*.h")
list(APPEND ORIGIN_HEADER_PATH_LIST
        ${ORIGIN_HEADER_PATH_LIST_arch_arm}
        ${ORIGIN_HEADER_PATH_LIST_arch_arm_st_hal}
        ${ORIGIN_HEADER_PATH_LIST_arch_avr}
        ${ORIGIN_HEADER_PATH_LIST_arch_cbuilder}
        ${ORIGIN_HEADER_PATH_LIST_arch_dos32}
        ${ORIGIN_HEADER_PATH_LIST_arch_linux}
        ${ORIGIN_HEADER_PATH_LIST_arch_qt}
        ${ORIGIN_HEADER_PATH_LIST_arch_visualcpp}
)
set(CONVERT_HEADER_PATH_LIST "")
# Если небыли переданы аргументы, то задаются стандартные значения
if(NOT DEFINED VERBOSE)
    set(VERBOSE False)
endif()
if(NOT DEFINED UTF8_MXSRCLIB_HEADERS_DIR)
    set(UTF8_MXSRCLIB_HEADERS_DIR "${SOURCE_DIR}/mxsrclib_utf-8")
endif()
# Цикл конвертации каждого файла
foreach (ORIGIN_HEADER_PATH ${ORIGIN_HEADER_PATH_LIST})
    set(CONVERT_HEADER_PATH "")
    # Расчет пути сконвертированного файла на основе переменной UTF8_MXSRCLIB_HEADERS_DIR и пути внутри директории mxsrclib
    string(FIND "${ORIGIN_HEADER_PATH}" "mxsrclib" INDEX_START REVERSE)
    if(${VERBOSE})
        message(STATUS "Конвертируемый файл: ${ORIGIN_HEADER_PATH}")
    endif()
    string(SUBSTRING ${ORIGIN_HEADER_PATH} ${INDEX_START} -1 SHORT_HEADER_PATH)
    string(FIND "${SHORT_HEADER_PATH}" "/" INDEX_START)
    math(EXPR INDEX_START "${INDEX_START}+1")
    string(SUBSTRING "${SHORT_HEADER_PATH}" ${INDEX_START} -1 SHORT_HEADER_PATH)
    if(${VERBOSE})
        message(STATUS "Путь внутри mxsrclib: ${SHORT_HEADER_PATH}")
    endif()
    set(CONVERT_HEADER_PATH "${UTF8_MXSRCLIB_HEADERS_DIR}/${SHORT_HEADER_PATH}")
    if(${VERBOSE})
        message(STATUS "Путь сохранения сконвертированного файла: ${CONVERT_HEADER_PATH}")
        # Визуальное разделение при выводе информации
        message("")
    endif()
    list(APPEND CONVERT_HEADER_PATH_LIST ${CONVERT_HEADER_PATH})
    # Команды конвертации
    if (CYGWIN)
        add_custom_command(
                OUTPUT "${CONVERT_HEADER_PATH}"
                COMMAND rm -f "${CONVERT_HEADER_PATH}"
                COMMAND iconv -f cp1251 -t utf-8 "${ORIGIN_HEADER_PATH}" >> "${CONVERT_HEADER_PATH}"
                DEPENDS ${ORIGIN_HEADER_PATH})
    elseif (UNIX)
        add_custom_command(
                OUTPUT ${CONVERT_HEADER_PATH}
                COMMAND iconv -f cp1251 -t utf-8 -o "${CONVERT_HEADER_PATH}" "${ORIGIN_HEADER_PATH}"
                DEPENDS ${ORIGIN_HEADER_PATH})
    else ()
        error("Unexpected platform")
    endif ()
endforeach ()
add_custom_target(convert_mxsrclib_headers ALL DEPENDS "${CONVERT_HEADER_PATH_LIST}")
set_target_properties(convert_mxsrclib_headers PROPERTIES CONVERTED_HEADERS_DIR "${UTF8_MXSRCLIB_HEADERS_DIR}")