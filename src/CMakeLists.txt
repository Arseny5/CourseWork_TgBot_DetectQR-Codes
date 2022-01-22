set(BINARY echobot)

add_library(QrToPng
        QrToPng.h
        QrToPng.cpp
)

set(QRTools stdc++fs tinypngoutput qr-code-generator)
target_link_libraries(QrToPng ${QRTools})

set(all_libs ${Libs} ${QRTools} QrToPng)

add_executable(${BINARY} source.cpp)
target_include_directories(${BINARY} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${BINARY} ${all_libs})


