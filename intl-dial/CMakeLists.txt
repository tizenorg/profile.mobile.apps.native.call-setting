SET(lib_name "intl-dial")
SET(RO_DBDIR "/usr/etc/call-setting")
SET(DBDIR "/opt/usr/dbspace")

INCLUDE(FindPkgConfig)
pkg_check_modules(${lib_name} REQUIRED dlog sqlite3 glib-2.0 db-util vconf)

FOREACH(flag ${${lib_name}_CFLAGS})
    SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fvisibility=hidden -fPIC -Wall -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

IF("${ARCH}" STREQUAL "arm")
    ADD_DEFINITIONS("-DTARGET")
ENDIF("${ARCH}" STREQUAL "arm")

ADD_DEFINITIONS("-DDBDIR=\"${DBDIR}\"")
ADD_DEFINITIONS("-DRO_DBDIR=\"${RO_DBDIR}\"")
ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DEXPORT_API=__attribute__((visibility(\"default\")))")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -Wl,--rpath=/usr/lib")

SET(PC_NAME ${lib_name})
SET(PC_REQUIRED ${pc_dependents})
SET(PC_LDFLAGS -l$"intl-dial")
SET(PC_CFLAGS -I\${includedir}/intl-dial)

ADD_LIBRARY(${lib_name} SHARED intl-dial.c)

SET_TARGET_PROPERTIES(${lib_name}
     PROPERTIES
     SOVERSION 0.1.0
     CLEAN_DIRECT_OUTPUT 1
)

TARGET_LINK_LIBRARIES(${lib_name} ${${lib_name}_LDFLAGS})

CONFIGURE_FILE(
	${lib_name}.pc.in
	${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}.pc
	@ONLY
)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/${lib_name}.pc DESTINATION ${SYSLIBDIR}/pkgconfig)

INSTALL(TARGETS ${lib_name} DESTINATION ${SYSLIBDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/intl-dial.h DESTINATION include/intl-dial)

EXEC_PROGRAM(sqlite3 ARGS "${CMAKE_CURRENT_SOURCE_DIR}/.intl-dial.db < ${CMAKE_CURRENT_SOURCE_DIR}/intl-dial.sql" RETURN_VALUE RET)
IF (${RET})
	MESSAGE(SEND_ERROR "sqlite3 failed")
ENDIF(${RET})

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/.intl-dial.db DESTINATION ${RO_DBDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/.intl-dial.db-journal DESTINATION ${RO_DBDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/.intl-dial.db DESTINATION ${DBDIR})
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/.intl-dial.db-journal DESTINATION ${DBDIR})


IF(UNIX)
ADD_CUSTOM_TARGET (distclean @echo cleaning for source distribution)
ADD_CUSTOM_COMMAND(
        DEPENDS clean
        COMMENT "distribution clean"
        COMMAND find
        ARGS    .
        -not -name config.cmake -and \(
        -name tester.c -or
        -name Testing -or
        -name CMakeFiles -or
        -name cmake.depends -or
        -name cmake.check_depends -or
        -name CMakeCache.txt -or
        -name cmake.check_cache -or
        -name *.cmake -or
        -name Makefile -or
        -name core -or
        -name core.* -or
        -name gmon.out -or
        -name install_manifest.txt -or
        -name *.pc -or
        -name *~ \)
        | grep -v TC | xargs rm -rf
        TARGET  distclean
        VERBATIM
)
ENDIF(UNIX)

