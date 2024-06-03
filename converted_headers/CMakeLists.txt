include(FindIconv)
if (NOT Iconv_FOUND)
    message("Error iconv not found")
endif ()

file(GLOB ORIGIN_HEADERS_FILES_LIST "${MXSRCLIB_DIR}/*.h")
set(CONVERTED_MXSRCLIB_HEADERS_DIR "${MXSRCLIB_DIR}/mxsrclib_utf8")

list(APPEND ORIGIN_HEADERS_FILES_LIST "${ORIGIN_HEADERS_FILES_LIST_arch}")

set(CONVERT_HEADER_PATH_LIST "")
# Цикл конвертации каждого файла
foreach (ORIGIN_HEADER_PATH ${ORIGIN_HEADERS_FILES_LIST})
    set(CONVERT_HEADER_PATH "")
    # Расчет пути сконвертированного файла на основе переменной CONVERTED_MXSRCLIB_HEADERS_DIR и пути внутри директории mxsrclib
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
    set(CONVERT_HEADER_PATH "${CONVERTED_MXSRCLIB_HEADERS_DIR}/${SHORT_HEADER_PATH}")
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

set(CONVERTED_MXSRCLIB_HEADERS
        ${CONVERTED_MXSRCLIB_HEADERS_DIR}
        "${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/${MXSRCLIB_ARCH}"
)
if(MXSRCLIB_ARCH STREQUAL "arm_st_hal")
    list(APPEND CONVERTED_MXSRCLIB_HEADERS "${CONVERTED_MXSRCLIB_HEADERS_DIR}/arch/arm")
endif()

# Создание таргета для подключения
add_library(converted_headers INTERFACE)
target_include_directories(converted_headers INTERFACE ${CONVERTED_MXSRCLIB_HEADERS})