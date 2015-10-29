CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(setting-call-efl C)

IF(_ENABLE_TIZEN_LITE_CODE)
ADD_DEFINITIONS("-D_TIZEN_LITE_CODE")
ADD_DEFINITIONS("-D_TIZEN_LITE_SOUNDS")
ADD_DEFINITIONS("-D_CALL_SET_TTS_SUPPORT")
ENDIF(_ENABLE_TIZEN_LITE_CODE)

SET(UG_SETTING_CALL setting-call-efl)
SET(PKG_NAME ${UG_SETTING_CALL})
SET(APP_ROOT_DIR /usr/apps/${PKG_NAME})
SET(LIB_DIR ${APP_ROOT_DIR}/lib/ug)
SET(DATA_DIR ${APP_ROOT_DIR}/shared/trusted)
SET(RES_DIR ${APP_ROOT_DIR}/res)
SET(EXEC_DIR ${APP_ROOT_DIR}/bin)
SET(TABLE_DIR ${RES_DIR}/tables)

IF("${CMAKE_BUILD_TYPE}" STREQUAL "")
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF("${CMAKE_BUILD_TYPE}" STREQUAL "")
MESSAGE("Build type: ${CMAKE_BUILD_TYPE}")


INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	appcore-efl
	elementary
	tapi
	vconf
	eina
	ecore-imf
	ecore-file
	dlog
	aul
	db-util
	ui-gadget-1
	contacts-service2
	msg-service
	capi-appfw-application
	capi-appfw-app-manager
	notification
	capi-content-media-content
	capi-media-metadata-extractor
	capi-system-system-settings
	capi-telephony
	efl-extension
)


INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/intl-dial)

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -Werror")
SET(CMAKE_C_FLAGS_DEBUG "-O0 -g")

ADD_DEFINITIONS("-DPREFIX=\"${CMAKE_INSTALL_PREFIX}\"")
ADD_DEFINITIONS("-DFACTORYFS=\"$ENV{PREFIX}\"")
ADD_DEFINITIONS("-DSLP_DEBUG")
ADD_DEFINITIONS("-D_CALL_SET_VT_VIDEO_CAST")

ADD_DEFINITIONS("-D_CALL_SET_FDN_SUPPORT")
ADD_LIBRARY(${UG_SETTING_CALL} SHARED
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

ADD_SUBDIRECTORY(intl-dial)
TARGET_LINK_LIBRARIES(${UG_SETTING_CALL} ${pkgs_LDFLAGS} intl-dial)

ADD_CUSTOM_TARGET(ug_effect.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/resource/images
		${CMAKE_CURRENT_SOURCE_DIR}/resource/ug_effect.edc ${CMAKE_BINARY_DIR}/ug_effect.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/ug_effect.edc
)
ADD_DEPENDENCIES(${UG_SETTING_CALL} ug_effect.edj)

ADD_CUSTOM_TARGET(call-setting.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/resource/images
		${CMAKE_CURRENT_SOURCE_DIR}/resource/call-setting.edc ${CMAKE_BINARY_DIR}/call-setting.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/resource/call-setting.edc
)
ADD_DEPENDENCIES(${UG_SETTING_CALL} call-setting.edj)


# edje theme
ADD_CUSTOM_TARGET(call-setting-theme.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/theme -sd
		${CMAKE_CURRENT_SOURCE_DIR}/theme/sounds
		${CMAKE_CURRENT_SOURCE_DIR}/theme/call-setting-theme.edc ${CMAKE_BINARY_DIR}/call-setting-theme.edj
		DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/theme/call-setting-theme.edc
)
ADD_DEPENDENCIES(${UG_SETTING_CALL} call-setting-theme.edj)

INSTALL(TARGETS ${UG_SETTING_CALL} DESTINATION ${LIB_DIR})
INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting.edj DESTINATION ${RES_DIR}/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/call-setting-theme.edj DESTINATION ${RES_DIR}/edje)
INSTALL(FILES ${CMAKE_BINARY_DIR}/ug_effect.edj DESTINATION ${RES_DIR}/edje)
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${RES_DIR}/images FILES_MATCHING PATTERN "*.png")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/images/ DESTINATION ${RES_DIR}/images FILES_MATCHING PATTERN "*.jpg")
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tables/ DESTINATION ${TABLE_DIR} FILES_MATCHING PATTERN "*.xml")
INSTALL(FILES ug.setting-call-efl.include DESTINATION /etc/smack/accesses2.d)
INSTALL(DIRECTORY DESTINATION ${DATA_DIR})

# i18n
ADD_SUBDIRECTORY(po)

#for Appcontrol
INSTALL(FILES ${CMAKE_SOURCE_DIR}/${UG_SETTING_CALL}.xml DESTINATION /usr/share/packages)
