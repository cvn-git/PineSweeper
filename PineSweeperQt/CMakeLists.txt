cmake_minimum_required(VERSION 3.19)
project(PineSweeperQt LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTORCC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets SvgWidgets)

qt_standard_project_setup()

qt_add_executable(PineSweeperQt
    WIN32 MACOSX_BUNDLE
    board.cpp
    board.h
    button_cell.cpp
    button_cell.h
    button_reset.cpp
    button_reset.h
    led_digit.cpp
    led_digit.h
    led_display.cpp
    led_display.h
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ../images.qrc
)

target_link_libraries(PineSweeperQt
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::SvgWidgets
)

include(GNUInstallDirs)

install(TARGETS PineSweeperQt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET PineSweeperQt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
