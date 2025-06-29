cmake_minimum_required(VERSION 3.16)
project(simple_pdf_viewer)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Pdf)
find_package(CURL REQUIRED)
find_package(Qt6 REQUIRED COMPONENTS Concurrent)

include_directories(/opt/homebrew/include)  
include_directories(include)
link_directories(/opt/homebrew/lib)

file(GLOB SRC
    "*.cpp"
    "*/*.cpp"
    "*/*/*.cpp"
)

add_executable(simple_pdf_viewer ${SRC})

target_link_libraries(simple_pdf_viewer
    Qt6::Pdf
    Qt6::Widgets
    Qt6::Concurrent
    ${CURL_LIBRARIES}
)