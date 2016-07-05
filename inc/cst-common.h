/*
 * Copyright (c) 2009-2015 Samsung Electronics Co., Ltd All Rights Reserved
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 */

#ifndef _CST_COMMON_H_
#define _CST_COMMON_H_

#include <Elementary.h>
#include <efl_extension.h>
#include <libintl.h>
#include <glib.h>
#include <app.h>
#include <TapiUtility.h>
#include <tapi_common.h>

#include "cst-debug.h"
#include <msg_types.h>

#ifndef CST_MODULE_EXPORT
#define CST_MODULE_EXPORT __attribute__ ((visibility("default")))
#endif

#define _EDJ(o)			elm_layout_edje_get(o)

#define THEME_PATH		"/edje/call-setting-theme.edj"
#define LOCAL_DIR		"/locale"

#define THEME_NAME		_cst_util_get_edj_path()
#define CST_LOCALE		_cst_util_get_locale_path()

#if !defined(APPNAME)
#define APPNAME "call-setting"
#endif

#if !defined(PACKAGE)
#define PACKAGE "org.tizen."APPNAME
#endif

#define CST_MAX_PASSWORD_LEN		4
#define CST_MAX_REJECT_NUM		30
#define CST_MAX_PREFIX_NUM		1
#define CST_MAX_REJECT_MESSAGE_NUM	5
#define CST_MAX_ITEM_TEXT_BUFFER_LEN	140
#define CST_MAX_REJECT_MSG_CHAR		160
#define CST_MAX_REJECT_MSG_CHAR_UNICODE		140
#define CST_MAX_PHONE_NUMBER_LEN	82
#define CST_MAX_AUTO_AREA_CODE_LEN	40
#define CST_MAX_CF_NUMBER_LEN	82
#define CST_MAX_PREFIX_NUMBER_LEN	20
#define CST_MAX_COUNTRY_PREFIX_NUMBER_LEN	6
#define CST_MAX_COUNTRY_NAME_LEN	14
#define CST_MAX_POPUP_MESSAGE_LEN 256
#define CST_NAVIFRAME_ITEM_POP_TIMER 0.15
#define CST_POPUP_TIMER 3.0
#define CST_SHOW_PROGRESSBAR_POPUP_TIMER 0.00000001
#define CST_MAX_NUM_AUTO_REJECT_LIST 100
#define CST_DISPLAY_NAME_LENGTH_MAX		(255+1)

/**
 * The key of request bundle for type.
 * \n Value : convert id to string by using \%d.
 */
#define CST_UG_BUNDLE_TYPE "type"
#define CST_UG_BUNDLE_VIEWTYPE "viewtype"
#define CST_UG_BUNDLE_KEYWORD "keyword"

/**
 * This enumeration defines the error codes returned from engine to UI
 */
typedef enum {
	CST_ERR_NONE = 0x00,						/**< numberOfPW_AttemptsViolation*/

	/*NETWORK ERRORS */
	/* Show message Not Available for Vodafone R11 */
	CST_ERR_UNKNOWN_SUBSCRIBER = 0x01,				/**<  unknownSubscriber  "Invalid User Number" */
	CST_ERR_BEARER_NOT_PROVISIONED = 0x02,				/**<  BearerServiceNotProvisioned "Request Rejected" */
	CST_ERR_TELESERVICE_NOT_PROVISIONED = 0x03,			/**<  TeleServiceNotProvisioned */
	CST_ERR_CALL_BARRED = 0x04,					/**<  CallBarred */
	CST_ERR_ILLEGAL_SS_OPER = 0x05,					/**<  illegalSS_Operation */
	CST_ERR_ERR_STATUS = 0x06,					/**<  ss_ErrorStatus */
	CST_ERR_NOT_AVAIL = 0x07,					/**<  ss_NotAvailable "Service not available" */
	CST_ERR_SUBSCRIPTION_VIOLATION = 0x08,				/**<  ss_SubscriptionViolation */
	CST_ERR_INCOMPATIBILITY = 0x09,					/**<  ss_Incompatibility */
	CST_ERR_FACILITY_NOT_SUPPORT = 0x0a,				/**<  FacilityNotSupported */
	CST_ERR_SYS_FAILURE = 0x0b,					/**<  "SystemFailure "Please try again" */
	CST_ERR_DATA_MISSING = 0x0c,					/**<  DataMissing */
	CST_ERR_UNEXPECTED_DATA = 0x0d,					/**<  UnexpectedDataValue 0x13 */
	CST_ERR_MPTY_PARTICIPANTS_EXCEED = 0x0e,			/**< maxNumberOfMPTY_ParticipantsExceeded */
	CST_ERR_RESOURCE_NOT_AVAIL = 0x0f,				/**<  ResourcesNotAvailable */
	/*Show message Password error for Vodafone R11 */
	CST_ERR_PW_REGIST_FAIL = 0x10,					/**< PasswordRegistrationFailure ( ex:password mismatch ) */
	CST_ERR_NEGATIVE_PW_CHECK = 0x11,				/**<  NegativePasswordCheck */
	/*Show message password error
	   this function has been locked, so please call customer center for Vodafone R11 */
	CST_ERR_PW_ATTEMPS_VIOLATION = 0x12,				/**<  numberOfPW_AttemptsViolation*/

	/*PHONE ERRORS */
	CST_ERR_NETWORK_API_FAILED = 0x13,				/**<  Telephony API call failed*/
	CST_ERR_DIVERT_TO_EMERGENCY_BLOCKED = 0x14,			/**<  Call diverting to an emergency number not allowed*/

	CST_ERR_UNKNOWN = 0xff,						/**<  unknown error*/

} CstErr_t;

enum {
	CST_SSTYPE_CF_UNCONDITIONAL,
	CST_SSTYPE_CF_BUSY,
	CST_SSTYPE_CF_NO_REPLY,
	CST_SSTYPE_CF_NOT_REACHABLE,
	CST_SSTYPE_CF_ALL,
	CST_SSTYPE_CF_ALL_CONDITIONAL,
	CST_SSTYPE_CW,
};

enum {
	CST_CF_CB_INTERROGATE_BY_NONE = 0,
	CST_CF_CB_INTERROGATE_BY_ACTIVATE = 1,
	CST_CF_CB_INTERROGATE_BY_DEACTIVATE = 2,
};

enum {
	CST_CLI_BY_NETWORK,
	CST_CLI_SHOW,
	CST_CLI_HIDE,
};

enum {
	CST_GENLIST_ENTRY_ITEM = -3,
	CST_GENLIST_ENTRY_ITEM2,
};

typedef enum {
	CST_SELECTED_SIM1,
	CST_SELECTED_SIM2,
} CstSimSlot_t;

enum {
	CST_CALLTYPE_VOICE,
	CST_CALLTYPE_VOICE_LINE2,
	CST_CALLTYPE_VIDEO,
	CST_CALLTYPE_ALL,
};

enum {
	CST_ACTION_ACTIVATE,
	CST_ACTION_DEACTIVATE,
	CST_ACTION_REGISTER,
	CST_ACTION_QUERY,
};

enum {
	CST_SS_STATE_PROGRESS_INIT,
	CST_SS_STATE_ON,
	CST_SS_STATE_OFF,
	CST_SS_STATE_PROGRESS,
};

enum {
	CST_ERROR_NONE,
	CST_ERROR_SERVICE_UNAVAILABLE,
	CST_ERROR_INCORRECT_OPERATION,
	CST_ERROR_INVALID_PASSWORD,
	CST_ERROR_PASSWORD_BLOCKED,
	CST_ERROR_ENTER_NUMBER,
	CST_ERROR_WRONG_NUMBER,
	CST_ERROR_ENTER_MESSAGE,
	CST_ERROR_SELECT_VALID_ENTRY,
	CST_ERROR_DUPLICATE_MESSAGE,
	CST_ERROR_REJECTED_BY_NETWORK,
	CST_ERROR_INSERT_SIM_CARD,
	CST_ERROR_CHANGE_FLIGHT_MODE,
	CST_ERROR_AUL_LAUNCH_ERROR,
	CST_ERROR_CHANGE_VOICEMAIL_NUMBER,
	CST_ERROR_OEM_NOT_SUPPORTED,
	CST_ERROR_OEM_NOT_SUPPORTED_USER_BUSY,
	CST_ERROR_OEM_NOT_SUPPORTED_NO_REPLY,
	CST_ERROR_OEM_NOT_SUPPORTED_OUT_OF_REACH,
	CST_ERROR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED,
	CST_ERROR_MAXIMUM_NUMBER_OF_CHARACTERS_REACHED_PD,
	CST_ERROR_MAXIMUM_DIGITS_OF_PHONE_NUMBER_REACHED,
	CST_ERROR_FDN_MODE_ACTIVE,
	CST_ERROR_NO_VOICEMAIL_NUM_CHANGED,
	CST_ERROR_UNKNOWN,
};

enum {
	CST_GL_ITEM_TEXT,
	CST_GL_ITEM_GROUP_INDEX,
	CST_GL_ITEM_1TEXT_ONOFF,
	CST_GL_ITEM_RADIO_1TEXT,
	CST_GL_ITEM_EXPANDABLE,
	CST_GL_ITEM_2TEXT_ONOFF_PROGRESS,
	CST_GL_ITEM_NONE,
};

enum {
	CST_DL_REJECT_CALL_WITH_MSG,
	CST_DL_NONE,
};

enum {
	CST_UG_REQ_MAIN_SCREEN = 0,
	CST_UG_REQ_REJECT_MESSAGES,
	CST_UG_REQ_ANSWER_END_CALLS,
	CST_UG_REQ_ADDITIONAL_SETTINGS,
	CST_UG_REQ_CALL_WAITING,
};

typedef enum {
	CST_IME_REJECT_MSG,
	CST_IME_CALL_FORWARD,
 	CST_IME_NONE,
} CstImeType_t;

typedef struct {
	int depth;
	int str_id;
	Elm_Genlist_Item_Type flags;
	int style;
	Evas_Object_Event_Cb func;
} CstGlItemDisplayInfo_t;

typedef struct _Item_Data {
	int index;
	int reject_id;
	int str_id;
	Elm_Object_Item *gl_item;
	Elm_Object_Item *parent_gl_item;
	char *label;
	Eina_Bool check;
	Evas_Object *radio;
	Evas_Object *eo_check;
	Eina_Bool is_activated;
	Eina_Bool is_unknown;
	Eina_List *list;
	char number[CST_MAX_PHONE_NUMBER_LEN + 1];
	char text[CST_MAX_ITEM_TEXT_BUFFER_LEN + 1];
	char name[CST_DISPLAY_NAME_LENGTH_MAX + 1];
	void *ad;
	int style;
	int cf_cb_request_by;
} CstGlItemData_t;

typedef struct {
	Evas_Object *win_main;
	Evas_Object *main_layout;
	Evas_Object *conform;

	Evas_Object *nf;
	Evas_Object *controlbar;
	Evas_Object *back_button;
	Evas_Object *backup_layout;
	Evas_Object *cf_btn;

	int ug_req_type;

	/* IME */
	Evas_Object *dg_entry;
	Evas_Object *dg_entry_contact_number;

	char entry_string[256];

	Eina_Bool b_expanded;

	Elm_Object_Item *gl_expanded_item;
	Elm_Object_Item *more_cst_navi_it;

	int genlist_editfield_initialized;

	/*All Calls */
	/*Show My Number */
	Evas_Object *rdg_show_my_number;

	/* Auto Reject */
	Eina_List *reject_list;

	/*voice, video call */
	int cf_state[4];
	Elm_Object_Item *cf_gl_item[4];
	char cf_number[CST_MAX_PHONE_NUMBER_LEN][4];
	Evas_Object *popup;

	/* Call waiting */
	int cw_state;
	CstGlItemData_t *cw_gl_item_data;
	Eina_Bool original_state;

	/* more-settings selected sim */
	int sel_sim;
	Elm_Object_Item *sim1_btn;
	Elm_Object_Item *sim2_btn;
	int call_type;
	int cf_flavour;
	Eina_List *req_queue;
	GSList *sim_req_list;
	Eina_Bool is_requesting;
	Evas_Object *backup_genlist;
	int kind_of_delete_list;

	/* Tapi Handle */
	TapiHandle *tapi_handle; /*Handle that point's to the SIM on which it will be operated on*/
	TapiHandle *sim1_tapi_handle;
	TapiHandle *sim2_tapi_handle;
	int sim_lock_status;
	int sim_lock_retry_account;
	int rej_msg_seg_size;

	Evas_Object *contact_btn;
	Eina_Bool check_state;

	Evas_Object *rejct_popup;
	Evas_Object *rejctlist_popup;
	Evas_Object *entry_count;
} CstAppData_t;

#endif	/* _CST_COMMON_H_ */

