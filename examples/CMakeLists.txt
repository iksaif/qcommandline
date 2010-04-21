# Copyright (C) 2009-2010 Corentin Chary <corentin.chary@gmail.com>
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

# Use it
INCLUDE( ${QT_USE_FILE} )

# Include the library include directories, and the current build directory (moc)
INCLUDE_DIRECTORIES(
	../src
	${CMAKE_CURRENT_BINARY_DIR}
)

SET(test_SRCS main.cpp test.cpp)
SET(test_MOC_HDRS test.h)

QT4_WRAP_CPP(MOC_SOURCE ${test_MOC_HDRS})

ADD_EXECUTABLE(
	test
	${test_SRCS}
	${MOC_SOURCE}
)

TARGET_LINK_LIBRARIES(
	test
	${QT_LIBRARIES}
	qcommandline
)

