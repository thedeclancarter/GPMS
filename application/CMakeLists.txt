cmake_minimum_required(VERSION 3.16)

project(gpms_ui VERSION 0.1 LANGUAGES CXX)

# Set the project root directory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Set Qt5 directory
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})
set(Qt5_DIR $ENV{Qt5_DIR})

# set virtual keyboard
# set(ENV{QT_IM_MODULE} "qtvirtualkeyboard")

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_PREFIX_PATH "C:/opencv/opencv/build/x64/vc16/lib" ${CMAKE_PREFIX_PATH})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Multimedia MultimediaWidgets)
find_package(Qt5QuickControls2 REQUIRED)
find_package(OpenCV REQUIRED)
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    find_package(Qt5 COMPONENTS VirtualKeyboard)
endif()

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
    ${PROJECT_ROOT}/src/pages/imageprojectionwindow.h
    ${PROJECT_ROOT}/src/pages/imageprojectionwindow.cpp
    ${PROJECT_ROOT}/src/pages/imageprojectionwindow.ui

    src/pages/calibration/calibrationPage.h
    src/pages/calibration/calibrationPage.cpp
    src/pages/calibration/calibrationPage.ui
    src/utils/image_utils.h
)

add_executable(gpms_ui ${PROJECT_SOURCES})

target_link_libraries(gpms_ui PRIVATE
    Qt5::Widgets
    Qt5::Multimedia
    Qt5::MultimediaWidgets
    Qt5::Core
    Qt5::Gui
     Qt5::Quick
    Qt5::QuickControls2
    # Qt5::VirtualKeyboard
    ${OpenCV_LIBS}
)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_ROOT}/src
    ${PROJECT_ROOT}/src/pages
    ${PROJECT_ROOT}/src/pages/sidebar_pages
)

if(APPLE)
    # Copy Info.plist to build directory
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Info.plist ${CMAKE_CURRENT_BINARY_DIR}/Info.plist COPYONLY)

    set_target_properties(gpms_ui PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_GUI_IDENTIFIER com.example.gpms_ui
        MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
        MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Info.plist
    )
endif()

if(WIN32)
    set_target_properties(gpms_ui PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

include(GNUInstallDirs)
install(TARGETS gpms_ui
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    target_link_libraries(gpms_ui PRIVATE
        Qt5::VirtualKeyboard
        Qt5::QuickWidgets
        Qt5::Qml
    )
endif()
