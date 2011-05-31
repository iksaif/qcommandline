# Copyright (C) 2009-2011 Corentin Chary <corentin.chary@gmail.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Library General Public
# License as published by the Free Software Foundation; either
# version 2 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Library General Public License for more details.
#
# You should have received a copy of the GNU Library General Public License
# along with this library; see the file COPYING.LIB.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
# Boston, MA 02110-1301, USA.

project(qcommandline)

# Stop cmake 2.6 from whining
cmake_minimum_required(VERSION 2.6)
if(COMMAND cmake_policy)
  cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

include(CMakeDependentOption)

# where to look first for cmake modules, before ${CMAKE_ROOT}/Modules/ is checked
if(CMAKE_SYSTEM_NAME MATCHES Linux)
  set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} /usr/share/apps/cmake/modules/)
endif()
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules/)

option(BUILD_SHARED_LIBS "build shared libs [default: on]" ON)
option(QCOMMANDLINE_BUILD_EXAMPLES "build examples [default: off]" OFF)

# compile in release mode with debug infos
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")
  add_definitions("-DQT_NO_DEBUG_OUTPUT")
endif("${CMAKE_BUILD_TYPE}" MATCHES "^Rel.*")

# Find Qt4
find_package( Qt4 REQUIRED )

set( QT_DONT_USE_QTGUI TRUE )

#add extra search paths for libraries and includes
set (LIB_SUFFIX "" CACHE STRING "Define suffix of directory name (32/64)" )
set (LIB_INSTALL_DIR "lib${LIB_SUFFIX}" CACHE STRING "Directory where lib will install")
set (INCLUDE_INSTALL_DIR "include" CACHE PATH "The directory the headers are installed in")
set (CMAKE_MODULES_INSTALL_DIR "share/apps/cmake/modules" CACHE PATH "The directory to install FindQCommandLine.cmake to")

set(QCOMMANDLINE_LIB_MAJOR_VERSION "0")
set(QCOMMANDLINE_LIB_MINOR_VERSION "2")
set(QCOMMANDLINE_LIB_PATCH_VERSION "0")

set(QCOMMANDLINE_LIB_VERSION_STRING "${QCOMMANDLINE_LIB_MAJOR_VERSION}.${QCOMMANDLINE_LIB_MINOR_VERSION}.${QCOMMANDLINE_LIB_PATCH_VERSION}")

set(LIBRARY_OUTPUT_PATH "${CMAKE_BINARY_DIR}/lib${LIB_SUFFIX}" )

# pkg-config
if (NOT WIN32)
  configure_file (${CMAKE_CURRENT_SOURCE_DIR}/QCommandLine.pc.in
                  ${CMAKE_CURRENT_BINARY_DIR}/QCommandLine.pc
                  @ONLY)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/QCommandLine.pc
           DESTINATION lib${LIB_SUFFIX}/pkgconfig
	   COMPONENT devel)
endif (NOT WIN32)

# Include the cmake file needed to use qt4
include( ${QT_USE_FILE} )

# Subdirs
add_subdirectory(src)
if (QCOMMANDLINE_BUILD_EXAMPLES)
  add_subdirectory(examples)
endif ()

add_subdirectory(cmake/modules)

set(CPACK_VERSION_MAJOR ${QCOMMANDLINE_LIB_MAJOR_VERSION})
set(CPACK_VERSION_MINOR ${QCOMMANDLINE_LIB_MINOR_VERSION})
set(CPACK_VERSION_PATCH ${QCOMMANDLINE_LIB_PATCH_VERSION})
set(CPACK_VERSION ${QCOMMANDLINE_LIB_VERSION_STRING})
set(CPACK_PACKAGE_VERSION ${QCOMMANDLINE_LIB_VERSION_STRING})
set(CPACK_PACKAGE_CONTACT "Corentin Chary <corentin.chary@gmail.com>")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/COPYING")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A Command line parser for Qt")
set(CPACK_PACKAGE_VENDOR "http://xf.iksaif.net/dev/qcommandline.html")
set(CPACK_SOURCE_IGNORE_FILES
  "~$"
  "^${PROJECT_SOURCE_DIR}/\\\\.git"
  "^${PROJECT_SOURCE_DIR}/build/")
#set(CPACK_INSTALL_CMAKE_PROJECTS "${CMAKE_CURRENT_BUILD_DIR};qcommandline;ALL;/")
set(CPACK_COMPONENTS_ALL libraries devel)
set(CPACK_COMPONENT_LIBRARIES_DISPLAY_NAME "Library")
set(CPACK_COMPONENT_DEVEL_DISPLAY_NAME "Development files")
set(CPACK_COMPONENT_LIBRARIES_DESCRIPTION "A Command line parser for Qt")
set(CPACK_COMPONENT_DEVEL_DESCRIPTION "Development files for Qcommandline")
set(CPACK_COMPONENT_DEVEL_DEPENDS libraries)
set(CPACK_PACKAGE_FILE_NAME "qcommandline-${CPACK_VERSION}-${CMAKE_SYSTEM_NAME}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "qcommandline-${CPACK_VERSION}")

if(NOT WIN32)
  set(CPACK_GENERATOR "DEB;TGZ;TBZ2")
  set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2")
  ## Debian specific ##
  set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt4-core (>= 4.6)")
  set(CPACK_DEBIAN_PACKAGE_BUILDS_DEPENDS "libqt4-dev (>= 4.6), cmake (>= 2.6)")
else()
  set(CPACK_GENERATOR "ZIP;NSIS")
  set(CPACK_SOURCE_GENERATOR "ZIP")
  set(CPACK_BINARY_CYGWIN "OFF")
  set(CPACK_SOURCE_CYGWIN "OFF")
  set(CPACK_PACKAGE_FILE_NAME "qcommandline-${CPACK_VERSION}-win32")
  set(CPACK_BINARY_NSIS "ON")
  set(CPACK_NSIS_MODIFY_PATH "OFF")
  set(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} QCommandLine")
  set(CPACK_NSIS_HELP_LINK "http://xf.iksaif.net/dev/qcommandline.html")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://xf.iksaif.net/dev/qcommandline.html")
  set(CPACK_NSIS_CONTACT "Corentin Chary <corentin.chary@gmail.com>")
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "qcommandline")
endif()

include(CPack)
