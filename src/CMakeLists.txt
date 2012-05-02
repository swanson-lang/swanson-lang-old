# -*- coding: utf-8 -*-
# ----------------------------------------------------------------------
# Copyright © 2011, RedJack, LLC.
# All rights reserved.
#
# Please see the LICENSE.txt file in this distribution for license
# details.
# ----------------------------------------------------------------------

include_directories(../include)

#-----------------------------------------------------------------------
# Build the library

set(LIBSWANSON_SRC
    libswanson/kernel.c
    libswanson/s0-evaluate.c
    libswanson/s0-parser.c
    libswanson/scope.c
)

add_library(libswanson SHARED ${LIBSWANSON_SRC})
set_target_properties(libswanson PROPERTIES
    OUTPUT_NAME swanson
    SOVERSION 0.0.0)
target_link_libraries(libswanson
    ${CORK_LIBRARIES}
)

install(TARGETS libswanson DESTINATION lib)

#-----------------------------------------------------------------------
# Build the command-line programs

set(SWAN_SRC
    swan/s0.c
    swan/swan.c
)

add_executable(swan ${SWAN_SRC})
target_link_libraries(swan libswanson)
install(TARGETS swan DESTINATION bin)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(swanson.pc.in swanson.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/swanson.pc
        DESTINATION lib/pkgconfig)
