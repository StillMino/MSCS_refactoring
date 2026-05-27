cmake_minimum_required(VERSION 3.16)
project(MSCS_SpecConfigurator VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Найти Qt6 или Qt5
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Xml Sql)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Xml Sql)

# Исходные файлы
set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/specification_manager.cpp
    src/export_config.cpp
    src/specifier_config.cpp
    src/xml_parser.cpp
    src/database_connector.cpp
    src/module_handler.cpp
)

# Заголовочные файлы
set(PROJECT_HEADERS
    include/mainwindow.h
    include/specification_manager.h
    include/export_config.h
    include/specifier_config.h
    include/xml_parser.h
    include/database_connector.h
    include/module_handler.h
    include/types.h
)

# UI файлы
set(PROJECT_UI
    ui/mainwindow.ui
    ui/export_config_dialog.ui
    ui/specifier_config_dialog.ui
    ui/profile_selector.ui
)

# Ресурсы
set(PROJECT_RESOURCES
    resources/resources.qrc
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_add_executable(MSCS_SpecConfigurator
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UI}
        ${PROJECT_RESOURCES}
    )
else()
    add_executable(MSCS_SpecConfigurator
        ${PROJECT_SOURCES}
        ${PROJECT_HEADERS}
        ${PROJECT_UI}
        ${PROJECT_RESOURCES}
    )
endif()

target_include_directories(MSCS_SpecConfigurator PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(MSCS_SpecConfigurator PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Xml
    Qt${QT_VERSION_MAJOR}::Sql
)

set_target_properties(MSCS_SpecConfigurator PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE FALSE
)

if(QT_VERSION_MAJOR GREATER_EQUAL 6)
    qt_finalize_executable(MSCS_SpecConfigurator)
endif()

# Установка
install(TARGETS MSCS_SpecConfigurator
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
