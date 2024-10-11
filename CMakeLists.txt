cmake_minimum_required(VERSION 3.16)

project(music_streaming_app VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Sql Network WebSockets Multimedia)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    include/mainwindow.h
    ui/mainwindow.ui
    ui/rooms.ui
    include/autorization.h
    src/autorization.cpp
    include/rooms.h
    src/rooms.cpp
    include/main_page.h
    src/main_page.cpp
    include/room_page.h
    src/room_page.cpp
    include/qlabel_clicked.h
    src/qlabel_clicked.cpp
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(music_streaming_app
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    if(ANDROID)
        add_library(music_streaming_app SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(music_streaming_app
            ${PROJECT_SOURCES}
        )
    endif()
endif()

# Link Qt Libraries
target_link_libraries(music_streaming_app PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Sql
    Qt${QT_VERSION_MAJOR}::Network
    Qt${QT_VERSION_MAJOR}::WebSockets
    Qt${QT_VERSION_MAJOR}::Multimedia
)

find_package(Qt6 REQUIRED COMPONENTS Gui)
target_link_libraries(music_streaming_app PRIVATE Qt${QT_VERSION_MAJOR}::Gui)

# Set the Bundle ID Option for macOS
set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.music_streaming_app)
set_target_properties(music_streaming_app PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS music_streaming_app
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(music_streaming_app)
endif()
