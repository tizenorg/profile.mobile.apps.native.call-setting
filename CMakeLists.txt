CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${PKG_NAME} C)

SET(EDJDIR "${APP_RES_EDJE_DIR}")
SET(IMAGEDIR "${APP_RES_IMAGE_DIR}")
SET(LOCALEDIR "${APP_RES_LOCALE_DIR}")
SET(EDJE_CC_DEFINITIONS "")

ADD_DEFINITIONS("-DEDJDIR=\"${EDJDIR}\"")
ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	appcore-efl
	capi-appfw-app-manager
	capi-appfw-application
	capi-media-metadata-extractor
	capi-system-system-settings
	capi-telephony
	contacts-service2
	dlog
	ecore-imf
	efl-extension
	eina
	elementary
	feedback
	libtzplatform-config
	msg-service
	notification
	tapi
	ui-gadget-1
	vconf
)


INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/inc)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_DEFINITIONS("-DUGNAME=\"${LIB_NAME}\"")

ADD_LIBRARY(${LIB_NAME} SHARED
	./src/cst-call-setting.c
	./src/cst-main.c
	./src/cst-common-string.c
	./src/cst-widget.c
	./src/cst-tapi-request.c
	./src/cst-answering.c
	./src/cst-forwarding.c
	./src/cst-forwarding-ime.c
	./src/cst-reject-msg.c
	./src/cst-reject-msg-ime.c
	./src/cst-delete-list.c
	./src/cst-util.c
	./src/cst-more-call-setting.c
	./src/cst-editfield.c
)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${pkgs_LDFLAGS})

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/call-setting-theme.edj
	COMMAND edje_cc -no-save -id ${CMAKE_SOURCE_DIR}/res/edje_images
	${EDJE_CC_DEFINITIONS}
	${CMAKE_SOURCE_DIR}/res/edje/call-setting-theme.edc ${CMAKE_BINARY_DIR}/call-setting-theme.edj
	DEPENDS ${CMAKE_SOURCE_DIR}/res/edje/call-setting-theme.edc
)
ADD_CUSTOM_TARGET(call-setting-theme.edje ALL DEPENDS ${CMAKE_BINARY_DIR}/call-setting-theme.edj)

INSTALL(TARGETS ${LIB_NAME} DESTINATION ${APP_LIB_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting-theme.edj DESTINATION ${EDJDIR})
INSTALL(DIRECTORY DESTINATION ${APP_DATA_DIR})

# i18n
ADD_SUBDIRECTORY(res/po)

#for Appcontrol
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PKG_NAME}.xml DESTINATION ${SHARE_PACKAGES_DIR})
