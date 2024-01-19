cmake_minimum_required(VERSION 3.16)
project(books LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

include_directories(
    "C:/msys64/mingw64/include"
)
link_directories(
    "C:/msys64/mingw64/lib"
)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Sql
    Widgets
)
qt_standard_project_setup()

qt_add_executable(books
    bookwindow.cpp
    bookwindow.h
    bookwindow.ui
    main.cpp
    row.h
    mycustomdelegate.h
)
target_link_libraries(books PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets
    shlwapi
    urlmon
    xlsxio_read
)
