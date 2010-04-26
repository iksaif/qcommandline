# Find QCOMMANDLINE - WEBDAV handling library for Qt
#
# This module defines
#  QCOMMANDLINE_FOUND - whether the qsjon library was found
#  QCOMMANDLINE_LIBRARIES - the qcommandline library
#  QCOMMANDLINE_INCLUDE_DIR - the include path of the qcommandline library
#

if (QCOMMANDLINE_INCLUDE_DIR AND QCOMMANDLINE_LIBRARIES)

  # Already in cache
  set (QCOMMANDLINE_FOUND TRUE)

else (QCOMMANDLINE_INCLUDE_DIR AND QCOMMANDLINE_LIBRARIES)

  if (NOT WIN32)
    # use pkg-config to get the values of QCOMMANDLINE_INCLUDE_DIRS
    # and QCOMMANDLINE_LIBRARY_DIRS to add as hints to the find commands.
    include (FindPkgConfig)
    pkg_check_modules (QCOMMANDLINE QCommandLine>=0.1)
  endif (NOT WIN32)

  find_library (QCOMMANDLINE_LIBRARIES
    NAMES
    qcommandline
    PATHS
    ${QCOMMANDLINE_LIBRARY_DIRS}
    ${LIB_INSTALL_DIR}
    ${KDE4_LIB_DIR}
  )

  find_path (QCOMMANDLINE_INCLUDE_DIR
    NAMES
    qcommandline.h
    PATH_SUFFIXES
    qcommandline
    PATHS
    ${QCOMMANDLINE_INCLUDE_DIRS}
    ${INCLUDE_INSTALL_DIR}
    ${KDE4_INCLUDE_DIR}
  )

  include(FindPackageHandleStandardArgs)
  find_package_handle_standard_args(QCOMMANDLINE DEFAULT_MSG QCOMMANDLINE_LIBRARIES QCOMMANDLINE_INCLUDE_DIR)

endif (QCOMMANDLINE_INCLUDE_DIR AND QCOMMANDLINE_LIBRARIES)
