cmake_minimum_required(VERSION 3.16)

project(gpms_ui VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui

        src/pages/sidebar_pages/userpage.h src/pages/sidebar_pages/userpage.cpp src/pages/sidebar_pages/userpage.ui
        src/pages/sidebar_pages/favoritespage.h src/pages/sidebar_pages/favoritespage.cpp src/pages/sidebar_pages/favoritespage.ui
        src/pages/sidebar_pages/settingspage.h src/pages/sidebar_pages/settingspage.cpp src/pages/sidebar_pages/settingspage.ui

        src/pages/createpage.h src/pages/createpage.cpp src/pages/createpage.ui


        src/pages/picturepage.h src/pages/picturepage.cpp src/pages/picturepage.ui
        src/pages/acceptpicturepage.h src/pages/acceptpicturepage.cpp src/pages/acceptpicturepage.ui
        src/pages/sensitivitypage.h src/pages/sensitivitypage.cpp src/pages/sensitivitypage.ui
        src/pages/textvisionpage.h src/pages/textvisionpage.cpp src/pages/textvisionpage.ui
        src/pages/pickimagespage.h src/pages/pickimagespage.cpp src/pages/pickimagespage.ui
        src/pages/projectpage.h src/pages/projectpage.cpp src/pages/projectpage.ui

        resources/images.qrc
        resources/styles.qrc
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

target_link_libraries(gpms_ui PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

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
)

include(GNUInstallDirs)
install(TARGETS gpms_ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(gpms_ui)
endif()
