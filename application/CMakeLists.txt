cmake_minimum_required(VERSION 3.16)

project(gpms_ui VERSION 0.1 LANGUAGES CXX)

# Set the project root directory
set(PROJECT_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# Set Qt5 directory
set(CMAKE_PREFIX_PATH $ENV{Qt5_DIR})
set(Qt5_DIR $ENV{Qt5_DIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
#set(CMAKE_PREFIX_PATH "C:/opencv/opencv/build/x64/vc16/lib" ${CMAKE_PREFIX_PATH})


if(APPLE)
    set(OpenCV_DIR "/opt/homebrew/Cellar/opencv/4.10.0_12/lib/cmake/opencv4")
    include_directories("/opt/homebrew/Cellar/opencv/4.10.0_12/include/opencv4")
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Core Multimedia MultimediaWidgets Quick QuickWidgets)
find_package(Qt5QuickControls2 REQUIRED)
find_package(OpenCV REQUIRED)

set(PROJECT_SOURCES
    ${PROJECT_ROOT}/src/main.cpp
    ${PROJECT_ROOT}/src/windows/mainwindow.cpp
    ${PROJECT_ROOT}/src/windows/mainwindow.h
    ${PROJECT_ROOT}/src/windows/mainwindow.ui
    ${PROJECT_ROOT}/src/windows/imageprojectionwindow.h
    ${PROJECT_ROOT}/src/windows/imageprojectionwindow.cpp
    ${PROJECT_ROOT}/src/windows/imageprojectionwindow.ui

    # Sidebar module; artifacts of early prototype, left in to be iterated on
    # ${PROJECT_ROOT}/src/pages/sidebarPages/userpage.h
    # ${PROJECT_ROOT}/src/pages/sidebarPages/userpage.cpp
    # ${PROJECT_ROOT}/src/pages/sidebarPages/userpage.ui
    # ${PROJECT_ROOT}/src/pages/sidebarPages/favoritespage.h
    # ${PROJECT_ROOT}/src/pages/sidebarPages/favoritespage.cpp
    # ${PROJECT_ROOT}/src/pages/sidebarPages/favoritespage.ui
    # ${PROJECT_ROOT}/src/pages/sidebarPages/settingspage.h
    # ${PROJECT_ROOT}/src/pages/sidebarPages/settingspage.cpp
    # ${PROJECT_ROOT}/src/pages/sidebarPages/settingspage.ui

    ${PROJECT_ROOT}/src/pages/calibration/calibrationPage.h
    ${PROJECT_ROOT}/src/pages/calibration/calibrationPage.cpp
    ${PROJECT_ROOT}/src/pages/calibration/calibrationPage.ui

    ${PROJECT_ROOT}/src/pages/create/createpage.h
    ${PROJECT_ROOT}/src/pages/create/createpage.cpp
    ${PROJECT_ROOT}/src/pages/create/createpage.ui

    ${PROJECT_ROOT}/src/pages/pickImages/pickimagespage.h
    ${PROJECT_ROOT}/src/pages/pickImages/pickimagespage.cpp
    ${PROJECT_ROOT}/src/pages/pickImages/pickimagespage.ui
    ${PROJECT_ROOT}/src/pages/pickImages/clickableframe.cpp
    ${PROJECT_ROOT}/src/pages/pickImages/clickableframe.h

    ${PROJECT_ROOT}/src/pages/project/projectpage.h
    ${PROJECT_ROOT}/src/pages/project/projectpage.cpp
    ${PROJECT_ROOT}/src/pages/project/projectpage.ui

    ${PROJECT_ROOT}/src/pages/sensitivity/sensitivitypage.h
    ${PROJECT_ROOT}/src/pages/sensitivity/sensitivitypage.cpp
    ${PROJECT_ROOT}/src/pages/sensitivity/sensitivitypage.ui

    ${PROJECT_ROOT}/src/pages/textVision/textvisionpage.h
    ${PROJECT_ROOT}/src/pages/textVision/textvisionpage.cpp
    ${PROJECT_ROOT}/src/pages/textVision/textvisionpage.ui

    ${PROJECT_ROOT}/src/utils/image_utils.h

    ${PROJECT_ROOT}/resources/images.qrc
    ${PROJECT_ROOT}/resources/styles.qrc
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
        Qt5::QuickWidgets
        Qt5::Qml
    )
endif()
