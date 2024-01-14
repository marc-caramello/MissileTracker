cmake_minimum_required(VERSION 3.16)
project(books LANGUAGES CXX)

include_directories(
    "C:\\msys64\\mingw64\\include"
)
find_package(Qt6 REQUIRED COMPONENTS
    Core
    Gui
    Sql
    Widgets
)
qt_standard_project_setup()

qt_add_executable(books
    bookdelegate.cpp bookdelegate.h
    bookwindow.cpp bookwindow.h bookwindow.ui
    initdb.h
    main.cpp
)
target_link_libraries(books PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Sql
    Qt6::Widgets

    shlwapi
    urlmon
)
