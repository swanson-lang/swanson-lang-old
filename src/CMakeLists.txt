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
    libswanson/foo.c
)

add_library(libswanson SHARED ${LIBSWANSON_SRC})
set_target_properties(libswanson PROPERTIES
    OUTPUT_NAME swanson
    SOVERSION 0.0.0)
target_link_libraries(libswanson
    ${CORK_LIBRARIES}
)

install(TARGETS libswanson
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)

#-----------------------------------------------------------------------
# Generate the pkg-config file

set(prefix ${CMAKE_INSTALL_PREFIX})
configure_file(swanson.pc.in swanson.pc)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/swanson.pc
        DESTINATION lib/pkgconfig)
