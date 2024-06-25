include(FindIconv)
if (NOT Iconv_FOUND)
    message("Не найден iconv")
endif ()

file(GLOB ORIGIN_HEADERS_FILES_LIST "${MXSRCLIB_DIR}/*.h")
set(CONVERTED_MXSRCLIB_HEADERS_DIR "${MXSRCLIB_DIR}/utf8_headers")

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
    string(REGEX REPLACE ".*mxsrclib." "" SHORT_HEADER_PATH "${ORIGIN_HEADER_PATH}")
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
                DEPENDS ${ORIGIN_HEADER_PATH}
        )
    elseif (UNIX)
        add_custom_command(
                OUTPUT ${CONVERT_HEADER_PATH}
                COMMAND iconv -f cp1251 -t utf-8 -o "${CONVERT_HEADER_PATH}" "${ORIGIN_HEADER_PATH}"
                DEPENDS ${ORIGIN_HEADER_PATH}
        )
    else ()
        error("Неподдерживаемая платформа")
    endif ()
endforeach ()

# Создание таргета для подключения
add_custom_target(utf8_headers ALL DEPENDS "${CONVERT_HEADER_PATH_LIST}")
set_target_properties(utf8_headers PROPERTIES CONVERTED_HEADERS_DIR ${CONVERTED_MXSRCLIB_HEADERS_DIR})
