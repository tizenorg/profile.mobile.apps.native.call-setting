CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(call-setting C)

SET(PKG_NAME ${APP_PKG_NAME})
SET(BIN_DIR ${APP_BIN_DIR})
SET(RES_DIR ${APP_RES_DIR})
SET(LOCALE_DIR "${RES_DIR}/locale")
SET(EDJ_DIR "${RES_DIR}/edje")

SET(INC_DIR "${CMAKE_SOURCE_DIR}/inc")
SET(EDC_IMG_DIR "${CMAKE_SOURCE_DIR}/res/edje_images")
SET(EDJE_DIR "${CMAKE_SOURCE_DIR}/res/edje")

SET(EDJE_CC_DEFINITIONS "")

FILE(GLOB SRCS "src/*.c")

INCLUDE_DIRECTORIES(${INC_DIR})

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
	vconf
)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIE ${EXTRA_CFLAGS} -Wall -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} "-pie" ${pkgs_LDFLAGS})

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/call-setting-theme.edj
	COMMAND edje_cc -id ${EDC_IMG_DIR}
	${EDJE_CC_DEFINITIONS}
	${EDJE_DIR}/call-setting-theme.edc ${CMAKE_BINARY_DIR}/call-setting-theme.edj
	DEPENDS ${EDJE_DIR}/call-setting-theme.edc
)
ADD_CUSTOM_TARGET(call-setting-theme.edje ALL DEPENDS ${CMAKE_BINARY_DIR}/call-setting-theme.edj)

INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting-theme.edj DESTINATION ${EDJ_DIR})

INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${BIN_DIR})
INSTALL(DIRECTORY DESTINATION ${DATADIR})

# i18n
ADD_SUBDIRECTORY(res/po)

#for Appcontrol
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PKG_NAME}.xml DESTINATION ${APP_SHARE_PACKAGES_DIR})
