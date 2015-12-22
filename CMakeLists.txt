CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(${PKG_NAME} C)

IF(_ENABLE_TIZEN_LITE_CODE)
ADD_DEFINITIONS("-D_TIZEN_LITE_CODE")
ADD_DEFINITIONS("-D_TIZEN_LITE_SOUNDS")
ADD_DEFINITIONS("-D_CALL_SET_TTS_SUPPORT")
ENDIF(_ENABLE_TIZEN_LITE_CODE)

SET(TABLE_DIR ${APP_RES_DIR}/tables)

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
	libtzplatform-config
	msg-service
	notification
	tapi
	ui-gadget-1
	vconf
)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_DEFINITIONS("-DUGNAME=\"${PKG_NAME}\"")
ADD_DEFINITIONS("-DRES_DIR=\"${APP_RES_DIR}\"")
ADD_DEFINITIONS("-DDATA_DIR=\"${APP_DATA_DIR}\"")


ADD_DEFINITIONS("-D_CALL_SET_FDN_SUPPORT")
ADD_LIBRARY(${LIB_NAME} SHARED
	./src/cst-call-setting.c
	./src/cst-main.c
	./src/cst-common-string.c
	./src/cst-widget.c
	./src/cst-tapi-request.c
	./src/cst-answering.c
	./src/cst-forwarding.c
	./src/cst-forwarding-ime.c
	./src/cst-barring.c
	./src/cst-reject-msg.c
	./src/cst-reject-msg-ime.c
	./src/cst-delete-list.c
	./src/cst-util.c
	./src/cst-more-call-setting.c
	./src/cst-fdn-setting.c
	./src/cst-fdn-list.c
	./src/cst-fdn-contact.c
	./src/cst-editfield.c
)

TARGET_LINK_LIBRARIES(${LIB_NAME} ${pkgs_LDFLAGS})

ADD_CUSTOM_TARGET(ug_effect.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/resource/images
		${CMAKE_CURRENT_SOURCE_DIR}/resource/ug_effect.edc ${CMAKE_BINARY_DIR}/ug_effect.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/ug_effect.edc
)
ADD_DEPENDENCIES(${LIB_NAME} ug_effect.edj)

ADD_CUSTOM_TARGET(call-setting.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/resource/images
		${CMAKE_CURRENT_SOURCE_DIR}/resource/call-setting.edc ${CMAKE_BINARY_DIR}/call-setting.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/call-setting.edc
)
ADD_DEPENDENCIES(${LIB_NAME} call-setting.edj)


# edje theme
ADD_CUSTOM_TARGET(call-setting-theme.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/theme -sd
		${CMAKE_CURRENT_SOURCE_DIR}/theme/sounds
		${CMAKE_CURRENT_SOURCE_DIR}/theme/call-setting-theme.edc ${CMAKE_BINARY_DIR}/call-setting-theme.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/theme/call-setting-theme.edc
)
ADD_DEPENDENCIES(${LIB_NAME} call-setting-theme.edj)

INSTALL(TARGETS ${LIB_NAME} DESTINATION ${APP_LIB_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting.edj DESTINATION ${APP_RES_DIR}/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting-theme.edj DESTINATION ${APP_RES_DIR}/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ug_effect.edj DESTINATION ${APP_RES_DIR}/edje)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${APP_RES_DIR}/images FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${APP_RES_DIR}/images FILES_MATCHING PATTERN "*.jpg")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tables/ DESTINATION ${TABLE_DIR} FILES_MATCHING PATTERN "*.xml")
INSTALL(DIRECTORY DESTINATION ${APP_DATA_DIR})

# i18n
ADD_SUBDIRECTORY(po)

#for Appcontrol
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${PKG_NAME}.xml DESTINATION ${SHARE_PACKAGES_DIR})
