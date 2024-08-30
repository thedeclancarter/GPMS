cmake_minimum_required(VERSION 3.16)

project(gpms_ui VERSION 0.1 LANGUAGES CXX)

# Set the project root directory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 COMPONENTS Multimedia MultimediaWidgets REQUIRED)
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    ${PROJECT_ROOT}/src/main.cpp
        ${PROJECT_ROOT}/src/mainwindow.cpp
        ${PROJECT_ROOT}/src/mainwindow.h
        ${PROJECT_ROOT}/src/mainwindow.ui

        ${PROJECT_ROOT}/src/pages/sidebar_pages/userpage.h
        ${PROJECT_ROOT}/src/pages/sidebar_pages/userpage.cpp
        ${PROJECT_ROOT}/src/pages/sidebar_pages/userpage.ui
        ${PROJECT_ROOT}/src/pages/sidebar_pages/favoritespage.h
        ${PROJECT_ROOT}/src/pages/sidebar_pages/favoritespage.cpp
        ${PROJECT_ROOT}/src/pages/sidebar_pages/favoritespage.ui
        ${PROJECT_ROOT}/src/pages/sidebar_pages/settingspage.h
        ${PROJECT_ROOT}/src/pages/sidebar_pages/settingspage.cpp
        ${PROJECT_ROOT}/src/pages/sidebar_pages/settingspage.ui

        ${PROJECT_ROOT}/src/pages/createpage.h
        ${PROJECT_ROOT}/src/pages/createpage.cpp
        ${PROJECT_ROOT}/src/pages/createpage.ui

        ${PROJECT_ROOT}/src/pages/acceptpicturepage.h
        ${PROJECT_ROOT}/src/pages/acceptpicturepage.cpp
        ${PROJECT_ROOT}/src/pages/acceptpicturepage.ui
        ${PROJECT_ROOT}/src/pages/sensitivitypage.h
        ${PROJECT_ROOT}/src/pages/sensitivitypage.cpp
        ${PROJECT_ROOT}/src/pages/sensitivitypage.ui
        ${PROJECT_ROOT}/src/pages/textvisionpage.h
        ${PROJECT_ROOT}/src/pages/textvisionpage.cpp
        ${PROJECT_ROOT}/src/pages/textvisionpage.ui
        ${PROJECT_ROOT}/src/pages/pickimagespage.h
        ${PROJECT_ROOT}/src/pages/pickimagespage.cpp
        ${PROJECT_ROOT}/src/pages/pickimagespage.ui
        ${PROJECT_ROOT}/src/pages/projectpage.h
        ${PROJECT_ROOT}/src/pages/projectpage.cpp
        ${PROJECT_ROOT}/src/pages/projectpage.ui

        ${PROJECT_ROOT}/src/pages/takepicture.h
        ${PROJECT_ROOT}/src/pages/takepicture.cpp
        ${PROJECT_ROOT}/src/pages/takepicture.ui

        ${PROJECT_ROOT}/resources/images.qrc
        ${PROJECT_ROOT}/resources/styles.qrc
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(gpms_ui
            MANUAL_FINALIZATION
            ${PROJECT_SOURCES}

        )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET gpms_ui APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(gpms_ui SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(gpms_ui
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(gpms_ui PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt6::Multimedia
    Qt6::MultimediaWidgets
    Qt6::Core
    Qt6::Gui
    ${OpenCV_LIBS}
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gpms_ui)
endif()

set_target_properties(gpms_ui PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
    # $<$<PLATFORM_ID:Darwin>:MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist>
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${PROJECT_ROOT}/src
    ${PROJECT_ROOT}/src/pages
    ${PROJECT_ROOT}/src/pages/sidebar_pages
    ${CMAKE_BINARY_DIR}
)

if(APPLE)
    # Copy Info.plist to build directory
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/Info.plist COPYONLY)
endif()

include(GNUInstallDirs)
install(TARGETS gpms_ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gpms_ui)
endif()
