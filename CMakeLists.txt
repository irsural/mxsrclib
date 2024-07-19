cmake_minimum_required(VERSION 3.22)

add_library(mxsrclib STATIC)
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_sources(mxsrclib PRIVATE
            irsadc.cpp
            irsalg.cpp
            irsdsp.cpp
            irserror.cpp
            irslocale.cpp
            irsmath.cpp
            irsstring.cpp
            irsstrm.cpp
            irstime.cpp
            timer.cpp
            irsexcept.cpp
            irsint.cpp
    )
else()
    target_sources(mxsrclib PRIVATE
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
            irstime.cpp
            timer.cpp
    )
endif()

if(${MXSRCLIB_ARCH} STREQUAL qt)
    target_sources(mxsrclib PRIVATE
            arch/${MXSRCLIB_ARCH}/counter.cpp
    )
    target_link_libraries(mxsrclib psapi ws2_32)
elseif(${MXSRCLIB_ARCH} STREQUAL arm)
    target_sources(mxsrclib PRIVATE
            arch/${MXSRCLIB_ARCH}/armadc.cpp
            arch/${MXSRCLIB_ARCH}/armcfg.cpp
            arch/${MXSRCLIB_ARCH}/armspi.cpp
            arch/${MXSRCLIB_ARCH}/counter.cpp
            arch/${MXSRCLIB_ARCH}/irsarchint.cpp
            arch/${MXSRCLIB_ARCH}/irsstm32_usb_hid_hardflow.cpp
    )
else()
    message(FATAL_ERROR "Invalid arch")
endif()

target_include_directories(mxsrclib PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/utils/libconfig/"
        "${CMAKE_CURRENT_SOURCE_DIR}/arch/${MXSRCLIB_ARCH}/"
)
