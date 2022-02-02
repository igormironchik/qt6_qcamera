
cmake_minimum_required( VERSION 3.1 )

if( NOT CMAKE_BUILD_TYPE )
	set( CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build."
		FORCE)
endif( NOT CMAKE_BUILD_TYPE )

SET( CMAKE_CXX_STANDARD 14 )

SET( CMAKE_CXX_STANDARD_REQUIRED ON )

project( Qt6QCamera )

set( CMAKE_AUTOMOC ON )
set( CMAKE_AUTORCC ON )
set( CMAKE_AUTOUIC ON )

find_package( Qt6Core REQUIRED )
find_package( Qt6Qml REQUIRED )
find_package( Qt6Multimedia REQUIRED )
find_package( Qt6Gui REQUIRED )
find_package( Qt6Quick REQUIRED )
find_package( Qt6Widgets REQUIRED )

set( SRC frames.cpp
    frames.hpp
    main.cpp
    main.qml )

set( CMAKE_CXX_STANDARD 14 )

set( CMAKE_CXX_STANDARD_REQUIRED ON )

qt6_add_resources( SRC resources.qrc )

add_subdirectory( 3rdparty/libyuv )


include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/libyuv/include )

add_executable( qt6_qcamera WIN32 ${SRC} )

target_link_libraries( qt6_qcamera PUBLIC yuv ${CAMERA_JPEG} Qt::Qml Qt::Quick Qt::Multimedia Qt::Widgets Qt::Gui Qt::Core )
